// HOST-ONLY: OpenSL ES object model over SDL3. See opensl_host.hpp.
//
// The vtables mirror the SLOT OFFSETS the decompiled code calls (documented
// in nusound_voice_android.cpp): ObjectItf {Realize 0x0, GetInterface 0xc,
// Destroy 0x18}, EngineItf {CreateAudioPlayer 0x8, CreateOutputMix 0x1c},
// PlayItf {SetPlayState 0x4, GetPlayState 0x8, GetPosition 0xc,
// RegisterCallback 0x10, SetCallbackEventsMask 0x14},
// AndroidSimpleBufferQueueItf {Enqueue 0x0, Clear 0x4, GetState 0x8},
// VolumeItf {SetVolumeLevel 0xc, EnableStereoPosition 0x14,
// SetStereoPosition 0x1c}. Slots the game never calls stay NULL so an
// unexpected call faults instead of silently passing.
//
// The interface ids are opaque tokens in the decompiled code ((const void *)
// 0x00010001 ..); GetInterface compares the same token values. The engine
// capabilities interface reports no optional profiles/outputs, which makes
// the original's InitAudioDevice take the bare-device path (the optional
// output queries are gated on the profile bits and are skipped).

#include "opensl_host.hpp"

#include <SDL3/SDL.h>

#include <math.h>
#include <pthread.h>
#include <stddef.h>

#include "nu2api/nucore/fixed_width.h"
#include <stdlib.h>
#include <string.h>

// Resolves the owning object from an embedded interface area.
#define HOSTSL_CONTAINER_OF(ptr, type, member) ((type *)((char *)(ptr) - offsetof(type, member)))

namespace hostsl {

    namespace {

        // SL result codes (from SLresult's 1.0.1 table).
        const u32 SL_RESULT_SUCCESS = 0x00000000;
        const u32 SL_RESULT_PARAMETER_INVALID = 0x0000000b;
        const u32 SL_RESULT_BUFFER_INSUFFICIENT = 0x0000000c;
        const u32 SL_RESULT_CONTENT_UNSUPPORTED = 0x0000000e;

        // Interface id tokens — must match the anonymous-namespace constants in
        // nusound_voice_android.cpp and InitAudioDevice.
        const void *IID_PLAY = (const void *)0x00010001;
        const void *IID_BUFFER_QUEUE = (const void *)0x00010002;
        const void *IID_VOLUME = (const void *)0x00010003;
        const void *IID_ENGINE = (const void *)0x00010004;
        const void *IID_ENGINECAPABILITIES = (const void *)0x00010005;
        const void *IID_ENVIRONMENTALREVERB = (const void *)0x00010006;

        // SL object kinds (fake-internal dispatch for the shared ObjectItf).
        const u32 KIND_ENGINE = 1;
        const u32 KIND_MIX = 2;
        const u32 KIND_PLAYER = 3;

        // The device sink: everything is converted to 16-bit stereo.
        SDL_AudioSpec device_spec = {SDL_AUDIO_S16LE, 2, 48000};
        SDL_AudioStream *device_stream = NULL;

        struct Stats s_stats = {};

        // ---------------------------------------------------------------------------
        // vtable shapes (field order = slot order; pads are uncalled slots)
        // ---------------------------------------------------------------------------

        struct ObjectVTable {
            u32 (*realize)(void *, u32);                         // 0x00
            u32 (*resume)(void *);                               // 0x04
            u32 (*get_state)(void *, u32 *);                     // 0x08
            u32 (*get_interface)(void *, const void *, void **); // 0x0c
            void *pad_0x10[2];
            u32 (*destroy)(void *); // 0x18
        };

        struct EngineVTable {
            void *pad_0x00[2];
            u32 (*create_audio_player)(void *, void **, void *, void *, u32, const void **, const u32 *); // 0x08
            void *pad_0x0c[4];
            u32 (*create_output_mix)(void *, void **, u32, const void **, const u32 *); // 0x1c
        };

        struct EngineCapsVTable {
            u32 (*query_supported_profiles)(void *, u16 *);                   // 0x00
            u32 (*query_available_outputs)(void *, u32, u32 *, u32 *, u32 *); // 0x04
            u32 (*query_realtime)(void *, u32 *);                             // 0x08
        };

        // The play-event callback type, identical to the game's PlayerCallback.
        struct SLPlayItf_;
        typedef void (*PlayCallbackFn)(const SLPlayItf_ *const *, void *, u32);

        struct PlayVTable {
            void *pad_0x00;
            u32 (*set_play_state)(void *, u32);                       // 0x04
            u32 (*get_play_state)(void *, u32 *);                     // 0x08
            u32 (*get_position)(void *, u32 *);                       // 0x0c
            u32 (*register_callback)(void *, PlayCallbackFn, void *); // 0x10
            u32 (*set_callback_events_mask)(void *, u32);             // 0x14
        };

        struct QueueVTable {
            u32 (*enqueue)(void *, void *, u32); // 0x00
            u32 (*clear)(void *);                // 0x04
            u32 (*get_state)(void *, u32 *);     // 0x08
        };

        struct VolumeVTable {
            void *pad_0x00[3];
            u32 (*set_volume_level)(void *, i32); // 0x0c
            void *pad_0x10;
            u32 (*enable_stereo_position)(void *, u32); // 0x14
            void *pad_0x18;
            u32 (*set_stereo_position)(void *, i32); // 0x1c
        };

        // ---------------------------------------------------------------------------
        // objects — each interface area is a struct whose first member is the vtable
        // pointer (that is what the game's `*(void **)itf` loads); the ObjectItf is
        // always the first member so the shared Object vtable can find the kind tag.
        // ---------------------------------------------------------------------------

        struct EngineObject {
            ObjectVTable *object_vt; // the ObjectItf seen by the game
            u32 kind;
            struct {
                EngineVTable *vt;
            } engine_itf;
            struct {
                EngineCapsVTable *vt;
            } caps_itf;
            bool realized;
        };

        struct MixObject {
            ObjectVTable *object_vt;
            u32 kind;
            bool realized;
        };

        enum { PLAYER_MAX_QUEUE = 16 };

        struct Player {
            ObjectVTable *object_vt;
            u32 kind;
            struct {
                PlayVTable *vt;
            } play_itf;
            struct {
                QueueVTable *vt;
            } queue_itf;
            struct {
                VolumeVTable *vt;
            } volume_itf;

            bool realized;

            // format the game enqueues in (from the SLDataFormat_PCM source)
            u32 channels;
            u32 rate;
            u32 bits;

            // play state (SL_PLAYSTATE_*: 1 stopped, 2 paused, 3 playing)
            u32 play_state;
            PlayCallbackFn play_callback;
            void *callback_context;
            u32 callback_mask;

            // volume (millibels; the game sends log10(gain) * 2000)
            i32 volume_millibels;
            bool stereo_position_enabled;
            i32 stereo_position_milli;

            // SDL conversion stream (src = the game's PCM format, dst = device spec)
            SDL_AudioStream *stream;

            // device-side consumption bookkeeping
            u64 consumed_device_frames;
            // FIFO of outstanding buffer sizes (source bytes), consumed in order
            u32 queue_sizes[PLAYER_MAX_QUEUE];
            u32 queue_head;
            u32 queue_count;
        };

        // ---------------------------------------------------------------------------
        // static vtables
        // ---------------------------------------------------------------------------

        // ObjectItf (shared by engine / mix / player)
        u32 ObjectRealize(void *self, u32 async);
        u32 ObjectResume(void *self);
        u32 ObjectGetState(void *self, u32 *out);
        u32 ObjectGetInterface(void *self, const void *iid, void **out);
        u32 ObjectDestroy(void *self);

        // EngineItf / EngineCapabilitiesItf
        u32 EngineCreateAudioPlayer(void *self, void **player_object, void *audio_src, void *audio_sink,
                                    u32 num_interfaces, const void **interface_ids, const u32 *required);
        u32 EngineCreateOutputMix(void *self, void **mix_object, u32 num_interfaces, const void **interface_ids,
                                  const u32 *required);
        u32 EngineQuerySupportedProfiles(void *self, u16 *profiles);
        u32 EngineQueryAvailableOutputs(void *self, u32 max_outputs, u32 *output_ids, u32 *output_details,
                                        u32 *num_outputs);
        u32 EngineQueryRealtime(void *self, u32 *config);

        // PlayItf
        u32 PlaySetPlayState(void *self, u32 state);
        u32 PlayGetPlayState(void *self, u32 *state);
        u32 PlayGetPosition(void *self, u32 *millisec);
        u32 PlayRegisterCallback(void *self, PlayCallbackFn callback, void *context);
        u32 PlaySetCallbackEventsMask(void *self, u32 mask);

        // AndroidSimpleBufferQueueItf
        u32 QueueEnqueue(void *self, void *data, u32 size);
        u32 QueueClear(void *self);
        u32 QueueGetState(void *self, u32 *count);

        // VolumeItf
        u32 VolumeSetVolumeLevel(void *self, i32 level);
        u32 VolumeEnableStereoPosition(void *self, u32 enabled);
        u32 VolumeSetStereoPosition(void *self, i32 position);

        // device mix loop
        void DeviceMixCallback(void *userdata, SDL_AudioStream *stream, int additional, int total);

        const ObjectVTable s_object_vt = {
            ObjectRealize, ObjectResume, ObjectGetState, ObjectGetInterface, NULL, NULL, ObjectDestroy,
        };
        const EngineVTable s_engine_vt = {
            NULL, NULL, EngineCreateAudioPlayer, NULL, NULL, NULL, NULL, EngineCreateOutputMix,
        };
        const EngineCapsVTable s_caps_vt = {
            EngineQuerySupportedProfiles,
            EngineQueryAvailableOutputs,
            EngineQueryRealtime,
        };
        const PlayVTable s_play_vt = {
            NULL, PlaySetPlayState, PlayGetPlayState, PlayGetPosition, PlayRegisterCallback, PlaySetCallbackEventsMask,
        };
        const QueueVTable s_queue_vt = {
            QueueEnqueue,
            QueueClear,
            QueueGetState,
        };
        const VolumeVTable s_volume_vt = {
            NULL, NULL, NULL, VolumeSetVolumeLevel, NULL, VolumeEnableStereoPosition, NULL, VolumeSetStereoPosition,
        };

        // ---------------------------------------------------------------------------
        // shared registry (device callback thread vs NuMain thread)
        // ---------------------------------------------------------------------------

        pthread_mutex_t g_lock = PTHREAD_MUTEX_INITIALIZER;
        Player *g_players[32];
        u32 g_player_count = 0;

        void RegisterPlayer(Player *player) {
            pthread_mutex_lock(&g_lock);
            if (g_player_count < 32) {
                g_players[g_player_count++] = player;
                s_stats.players_created++;
            }
            pthread_mutex_unlock(&g_lock);
        }

        void RemovePlayer(Player *player) {
            pthread_mutex_lock(&g_lock);
            for (u32 i = 0; i < g_player_count; i++) {
                if (g_players[i] == player) {
                    g_players[i] = g_players[--g_player_count];
                    break;
                }
            }
            pthread_mutex_unlock(&g_lock);
            if (player->stream != NULL) {
                SDL_DestroyAudioStream(player->stream);
                player->stream = NULL;
            }
        }

        u32 GetConsumedSourceBytes(Player *player) {
            // Device frames pulled out of the SDL stream converted to the player's
            // source rate; derived from the total each time (no accumulation drift).
            const u64 consumed_src_frames = player->consumed_device_frames * (u64)player->rate / (u64)device_spec.freq;
            return (u32)(consumed_src_frames * (u64)(player->channels * (player->bits / 8)));
        }

        // ---------------------------------------------------------------------------
        // ObjectItf (shared by engine / mix / player)
        // ---------------------------------------------------------------------------

        u32 ObjectRealize(void *self, u32 async) {
            (void)async;
            switch (((ObjectVTable **)self)[0] == &s_object_vt ? ((u32 *)self)[1] : 0) {
                case KIND_ENGINE:
                    ((EngineObject *)self)->realized = true;
                    return SL_RESULT_SUCCESS;
                case KIND_MIX:
                    ((MixObject *)self)->realized = true;
                    return SL_RESULT_SUCCESS;
                case KIND_PLAYER:
                    ((Player *)self)->realized = true;
                    return SL_RESULT_SUCCESS;
                default:
                    return SL_RESULT_PARAMETER_INVALID;
            }
        }

        // SLObjectItf::Resume (object vtable slot 0x4). The engine's state poll uses
        // it to resume a suspended player object.
        u32 ObjectResume(void *self) {
            (void)self;
            return SL_RESULT_SUCCESS;
        }

        // SLObjectItf::GetState (object vtable slot 0x8): the realized state of the
        // object, not the play state.
        u32 ObjectGetState(void *self, u32 *out) {
            (void)self;
            *out = 1; // SL_OBJECT_STATE_REALIZED
            return SL_RESULT_SUCCESS;
        }

        u32 ObjectGetInterface(void *self, const void *iid, void **out) {
            *out = NULL;
            const u32 kind = ((u32 *)self)[1];

            if (kind == KIND_ENGINE) {
                EngineObject *engine = (EngineObject *)self;
                if (iid == IID_ENGINE) {
                    *out = &engine->engine_itf;
                    return SL_RESULT_SUCCESS;
                }
                if (iid == IID_ENGINECAPABILITIES) {
                    *out = &engine->caps_itf;
                    return SL_RESULT_SUCCESS;
                }
                return SL_RESULT_PARAMETER_INVALID;
            }

            if (kind == KIND_PLAYER) {
                Player *player = (Player *)self;
                if (iid == IID_PLAY) {
                    *out = &player->play_itf;
                    return SL_RESULT_SUCCESS;
                }
                if (iid == IID_BUFFER_QUEUE) {
                    *out = &player->queue_itf;
                    return SL_RESULT_SUCCESS;
                }
                if (iid == IID_VOLUME) {
                    *out = &player->volume_itf;
                    return SL_RESULT_SUCCESS;
                }
                return SL_RESULT_PARAMETER_INVALID;
            }

            // Output mix: the original tolerates both of its interface queries
            // failing (nothing in the transcribed scope reads the results).
            return SL_RESULT_PARAMETER_INVALID;
        }

        u32 ObjectDestroy(void *self) {
            const u32 kind = ((u32 *)self)[1];
            if (kind == KIND_PLAYER) {
                RemovePlayer((Player *)self);
                free(self);
                return SL_RESULT_SUCCESS;
            }
            if (kind == KIND_MIX) {
                free(self);
                return SL_RESULT_SUCCESS;
            }
            if (kind == KIND_ENGINE) {
                // Engine teardown shuts the device.
                if (device_stream != NULL) {
                    SDL_DestroyAudioStream(device_stream);
                    device_stream = NULL;
                }
                free(self);
                return SL_RESULT_SUCCESS;
            }
            return SL_RESULT_PARAMETER_INVALID;
        }

        // ---------------------------------------------------------------------------
        // EngineItf / EngineCapabilitiesItf
        // ---------------------------------------------------------------------------

        u32 EngineCreateAudioPlayer(void *self, void **player_object, void *audio_src, void *audio_sink,
                                    u32 num_interfaces, const void **interface_ids, const u32 *required) {
            (void)self;
            (void)audio_sink;
            (void)num_interfaces;
            (void)interface_ids;
            (void)required;

            *player_object = NULL;

            // audio_src = { SLDataLocator_AndroidSimpleBufferQueue, SLDataFormat_PCM }
            const u32 *locator = (const u32 *)((void **)audio_src)[0];
            const u32 *pcm = (const u32 *)((void **)audio_src)[1];
            if (locator[0] != 0x800007bdu) { // SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE
                return SL_RESULT_PARAMETER_INVALID;
            }
            if (pcm[0] != 2) { // SL_DATAFORMAT_PCM
                return SL_RESULT_CONTENT_UNSUPPORTED;
            }

            const u32 channels = pcm[1];
            const u32 rate = pcm[2] / 1000; // milli Hz -> Hz
            const u32 bits = pcm[3];

            if (channels != 1 && channels != 2) {
                return SL_RESULT_CONTENT_UNSUPPORTED;
            }
            if (bits != 8 && bits != 16 && bits != 24) {
                return SL_RESULT_CONTENT_UNSUPPORTED;
            }
            if (rate == 0) {
                return SL_RESULT_CONTENT_UNSUPPORTED;
            }

            Player *player = (Player *)calloc(1, sizeof(Player));
            if (player == NULL) {
                return SL_RESULT_PARAMETER_INVALID;
            }

            player->object_vt = (ObjectVTable *)&s_object_vt;
            player->kind = KIND_PLAYER;
            player->play_itf.vt = (PlayVTable *)&s_play_vt;
            player->queue_itf.vt = (QueueVTable *)&s_queue_vt;
            player->volume_itf.vt = (VolumeVTable *)&s_volume_vt;
            player->channels = channels;
            player->rate = rate;
            player->bits = bits;
            player->play_state = 1;
            player->volume_millibels = -32768; // muted until the first mix, like the voice

            SDL_AudioSpec src = {SDL_AUDIO_S16LE, (int)channels, (int)rate};
            if (bits == 8) {
                src.format = SDL_AUDIO_U8; // OpenSL 8-bit PCM is unsigned
            } else if (bits == 24) {
                src.format = SDL_AUDIO_S32LE; // expanded on Enqueue
            }
            player->stream = SDL_CreateAudioStream(&src, &device_spec);

            RegisterPlayer(player);
            *player_object = player;
            return SL_RESULT_SUCCESS;
        }

        u32 EngineCreateOutputMix(void *self, void **mix_object, u32 num_interfaces, const void **interface_ids,
                                  const u32 *required) {
            (void)self;
            (void)num_interfaces;
            (void)interface_ids;
            (void)required;

            // The output mix IS the SDL device: open it on first use.
            if (device_stream == NULL) {
                device_stream =
                    SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &device_spec, DeviceMixCallback, NULL);
                if (device_stream == NULL) {
                    *mix_object = NULL;
                    return SL_RESULT_PARAMETER_INVALID;
                }
                SDL_ResumeAudioStreamDevice(device_stream);
            }

            MixObject *mix = (MixObject *)calloc(1, sizeof(MixObject));
            if (mix == NULL) {
                return SL_RESULT_PARAMETER_INVALID;
            }
            mix->object_vt = (ObjectVTable *)&s_object_vt;
            mix->kind = KIND_MIX;
            *mix_object = mix;
            return SL_RESULT_SUCCESS;
        }

        u32 EngineQuerySupportedProfiles(void *self, u16 *profiles) {
            (void)self;
            // Advertise no optional profiles: the original skips its optional output
            // queries unless the android profile bit (0x4) is set.
            *profiles = 0;
            return SL_RESULT_SUCCESS;
        }

        u32 EngineQueryAvailableOutputs(void *self, u32 max_outputs, u32 *output_ids, u32 *output_details,
                                        u32 *num_outputs) {
            (void)self;
            (void)max_outputs;
            (void)output_ids;
            (void)output_details;
            *num_outputs = 0;
            return SL_RESULT_SUCCESS;
        }

        u32 EngineQueryRealtime(void *self, u32 *config) {
            (void)self;
            *config = 0;
            return SL_RESULT_SUCCESS;
        }

        // ---------------------------------------------------------------------------
        // PlayItf
        // ---------------------------------------------------------------------------

        u32 PlaySetPlayState(void *self, u32 state) {
            ((Player *)HOSTSL_CONTAINER_OF(self, Player, play_itf))->play_state = state;
            return SL_RESULT_SUCCESS;
        }

        u32 PlayGetPlayState(void *self, u32 *state) {
            *state = ((Player *)HOSTSL_CONTAINER_OF(self, Player, play_itf))->play_state;
            return SL_RESULT_SUCCESS;
        }

        u32 PlayGetPosition(void *self, u32 *millisec) {
            Player *player = (Player *)HOSTSL_CONTAINER_OF(self, Player, play_itf);
            const u64 consumed_src_frames = player->consumed_device_frames * (u64)player->rate / (u64)device_spec.freq;
            *millisec = (u32)(consumed_src_frames * 1000ULL / (u64)player->rate);
            return SL_RESULT_SUCCESS;
        }

        u32 PlayRegisterCallback(void *self, PlayCallbackFn callback, void *context) {
            Player *player = (Player *)HOSTSL_CONTAINER_OF(self, Player, play_itf);
            player->play_callback = callback;
            player->callback_context = context;
            return SL_RESULT_SUCCESS;
        }

        u32 PlaySetCallbackEventsMask(void *self, u32 mask) {
            ((Player *)HOSTSL_CONTAINER_OF(self, Player, play_itf))->callback_mask = mask;
            return SL_RESULT_SUCCESS;
        }

        // ---------------------------------------------------------------------------
        // AndroidSimpleBufferQueueItf
        // ---------------------------------------------------------------------------

        u32 QueueEnqueue(void *self, void *data, u32 size) {
            Player *player = (Player *)HOSTSL_CONTAINER_OF(self, Player, queue_itf);
            if (data == NULL || size == 0 || player->stream == NULL) {
                return SL_RESULT_PARAMETER_INVALID;
            }

            pthread_mutex_lock(&g_lock);
            if (player->queue_count == PLAYER_MAX_QUEUE) {
                pthread_mutex_unlock(&g_lock);
                return SL_RESULT_BUFFER_INSUFFICIENT;
            }

            if (player->bits == 24) {
                // 24-bit LE samples expand into 32-bit words for SDL.
                const u32 samples = size / 3;
                static u32 expanded[0x20000];
                if (samples > 0x20000) {
                    pthread_mutex_unlock(&g_lock);
                    return SL_RESULT_BUFFER_INSUFFICIENT;
                }
                const u8 *in = (const u8 *)data;
                for (u32 i = 0; i < samples; i++) {
                    const u32 value = (u32)in[i * 3 + 0] | ((u32)in[i * 3 + 1] << 8) | ((u32)in[i * 3 + 2] << 16);
                    expanded[i] = value << 8;
                }
                SDL_PutAudioStreamData(player->stream, expanded, (int)(samples * 4));
            } else {
                SDL_PutAudioStreamData(player->stream, data, (int)size);
            }

            player->queue_sizes[(player->queue_head + player->queue_count) % PLAYER_MAX_QUEUE] = size;
            player->queue_count++;
            s_stats.bytes_enqueued += size;
            pthread_mutex_unlock(&g_lock);
            return SL_RESULT_SUCCESS;
        }

        u32 QueueClear(void *self) {
            Player *player = (Player *)HOSTSL_CONTAINER_OF(self, Player, queue_itf);
            pthread_mutex_lock(&g_lock);
            if (player->stream != NULL) {
                SDL_ClearAudioStream(player->stream);
            }
            player->queue_head = 0;
            player->queue_count = 0;
            pthread_mutex_unlock(&g_lock);
            return SL_RESULT_SUCCESS;
        }

        // Drops queue entries the device fully consumed; returns the outstanding count.
        u32 QueueCountOutstanding(Player *player) {
            const u32 consumed = GetConsumedSourceBytes(player);
            u32 done_bytes = 0;
            u32 done = 0;
            for (u32 e = 0; e < player->queue_count; e++) {
                const u32 size = player->queue_sizes[(player->queue_head + e) % PLAYER_MAX_QUEUE];
                if ((u64)done_bytes + size <= consumed) {
                    done_bytes += size;
                    done++;
                } else {
                    break;
                }
            }
            player->queue_head = (player->queue_head + done) % PLAYER_MAX_QUEUE;
            player->queue_count -= done;
            return done;
        }

        u32 QueueGetState(void *self, u32 *count) {
            Player *player = (Player *)HOSTSL_CONTAINER_OF(self, Player, queue_itf);
            pthread_mutex_lock(&g_lock);
            QueueCountOutstanding(player);
            *count = player->queue_count;
            pthread_mutex_unlock(&g_lock);
            return SL_RESULT_SUCCESS;
        }

        // ---------------------------------------------------------------------------
        // VolumeItf
        // ---------------------------------------------------------------------------

        u32 VolumeSetVolumeLevel(void *self, i32 level) {
            ((Player *)HOSTSL_CONTAINER_OF(self, Player, volume_itf))->volume_millibels = level;
            return SL_RESULT_SUCCESS;
        }

        u32 VolumeEnableStereoPosition(void *self, u32 enabled) {
            ((Player *)HOSTSL_CONTAINER_OF(self, Player, volume_itf))->stereo_position_enabled = enabled != 0;
            return SL_RESULT_SUCCESS;
        }

        u32 VolumeSetStereoPosition(void *self, i32 position) {
            Player *player = (Player *)HOSTSL_CONTAINER_OF(self, Player, volume_itf);
            if (position < -1000 || position > 1000) {
                return SL_RESULT_PARAMETER_INVALID;
            }
            player->stereo_position_milli = position;
            return SL_RESULT_SUCCESS;
        }

        // ---------------------------------------------------------------------------
        // device mix loop
        // ---------------------------------------------------------------------------

        const u32 MIX_CHUNK_BYTES = 4096; // one pull per player per pass
        enum { FIRED_MAX = 64 };

        void DeviceMixCallback(void *userdata, SDL_AudioStream *stream, int additional, int total) {
            (void)userdata;
            (void)total;

            static u8 pull_buffer[MIX_CHUNK_BYTES];
            static f32 mix_acc[MIX_CHUNK_BYTES / 2]; // s16 stereo frame accumulator

            // Buffer-consumed events fire after the registry lock is dropped: the
            // handler takes the voice mutex, and Enqueue holds that mutex while
            // touching the registry, so taking it under g_lock would invert the order.
            Player *fired[FIRED_MAX];
            u32 fired_count = 0;

            pthread_mutex_lock(&g_lock);

            while (additional > 0) {
                u32 chunk = (additional < (int)MIX_CHUNK_BYTES) ? (u32)additional : MIX_CHUNK_BYTES;
                chunk &= ~3u; // whole s16 stereo frames
                if (chunk == 0) {
                    break;
                }
                const u32 frames = chunk / 4;

                memset(mix_acc, 0, (usize)chunk * 2);

                for (u32 i = 0; i < g_player_count; i++) {
                    Player *player = g_players[i];
                    if (player->play_state != 3 || player->stream == NULL) {
                        continue; // stopped/paused players keep their data queued
                    }

                    const u32 available = (u32)SDL_GetAudioStreamAvailable(player->stream);
                    const u32 take = (available < chunk ? available : chunk) & ~3u;
                    if (take == 0) {
                        // Underrun: nothing queued means the play head sits at the end
                        // of the last buffer, which is when real SL raises
                        // SL_PLAYEVENT_HEADATEND. The streaming refill starts here.
                        if (fired_count < FIRED_MAX) {
                            fired[fired_count++] = player;
                        }
                        continue;
                    }
                    const int got = SDL_GetAudioStreamData(player->stream, pull_buffer, (int)take);
                    if (got < 4) {
                        continue;
                    }

                    const f32 gain = powf(10.0f, (f32)player->volume_millibels / 2000.0f);
                    f32 left = gain;
                    f32 right = gain;
                    if (player->stereo_position_enabled && player->channels == 1) {
                        // SL stereo position: -1000 = hard left, +1000 = hard right.
                        const f32 pan = (f32)player->stereo_position_milli / 1000.0f;
                        left = gain * (1.0f - pan) * 0.5f;
                        right = gain * (1.0f + pan) * 0.5f;
                    }

                    const i16 *in = (const i16 *)pull_buffer;
                    for (u32 f = 0; f < (u32)got / 4; f++) {
                        mix_acc[f * 2 + 0] += (f32)in[f * 2 + 0] * left;
                        mix_acc[f * 2 + 1] += (f32)in[f * 2 + 1] * right;
                    }

                    player->consumed_device_frames += (u32)got / 4;
                    s_stats.bytes_consumed += (u32)got;

                    // Fire one event per fully consumed buffer (the buffer-queue
                    // callback in real SL drives the streaming refill off this).
                    const u32 done = QueueCountOutstanding(player);
                    for (u32 e = 0; e < done && fired_count < FIRED_MAX; e++) {
                        fired[fired_count++] = player;
                    }
                }

                i16 *out16 = (i16 *)pull_buffer;
                f64 sum = 0;
                for (u32 f = 0; f < frames * 2; f++) {
                    f32 sample = mix_acc[f];
                    if (sample > 32767.0f) {
                        sample = 32767.0f;
                    } else if (sample < -32768.0f) {
                        sample = -32768.0f;
                    }
                    out16[f] = (i16)sample;
                    sum += (f64)out16[f] * out16[f];
                }
                s_stats.last_mix_rms = (f32)sqrt(sum / (frames * 2));

                SDL_PutAudioStreamData(stream, out16, (int)chunk);
                additional -= (int)chunk;
            }

            pthread_mutex_unlock(&g_lock);

            for (u32 i = 0; i < fired_count; i++) {
                Player *player = fired[i];
                if (player->play_callback != NULL && (player->callback_mask & 1) != 0) {
                    // SL_PLAYEVENT_HEADATMARKER: the game's OnPlayerEvent handles bit 0.
                    void *itf_self = &player->play_itf;
                    player->play_callback((const SLPlayItf_ *const *)&itf_self, player->callback_context, 1);
                }
            }
        }

    } // namespace

    // ---------------------------------------------------------------------------
    // public entry
    // ---------------------------------------------------------------------------

    u32 slCreateEngine(void **engine_object, u32 num_options, void *options, u32 num_interfaces,
                       const void **interface_ids, const u32 *required) {
        (void)num_options;
        (void)options;
        (void)num_interfaces;
        (void)interface_ids;
        (void)required;

        *engine_object = NULL;
        EngineObject *engine = (EngineObject *)calloc(1, sizeof(EngineObject));
        if (engine == NULL) {
            return SL_RESULT_PARAMETER_INVALID;
        }
        engine->object_vt = (ObjectVTable *)&s_object_vt;
        engine->kind = KIND_ENGINE;
        engine->engine_itf.vt = (EngineVTable *)&s_engine_vt;
        engine->caps_itf.vt = (EngineCapsVTable *)&s_caps_vt;
        *engine_object = engine;
        return SL_RESULT_SUCCESS;
    }

    Stats GetStats() {
        pthread_mutex_lock(&g_lock);
        Stats stats = s_stats;
        stats.players_playing = 0;
        for (u32 i = 0; i < g_player_count; i++) {
            if (g_players[i]->play_state == 3) {
                stats.players_playing++;
            }
        }
        pthread_mutex_unlock(&g_lock);
        return stats;
    }

} // namespace hostsl
