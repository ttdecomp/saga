// HOST-ONLY: OpenSL ES object model over SDL3. See opensl.hpp.
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

#include "host/platform/opensl.hpp"

#include <SDL3/SDL.h>

#include <math.h>
#include <pthread.h>
#include <stddef.h>

#include "nu2api/nucore/fixed_width.h"
#include "nu2api/nusound/opensles_abi.hpp"
#include "decomp.h"
#include <stdlib.h>
#include <string.h>

// Resolves the owning object from an embedded interface area.
#define HOSTSL_CONTAINER_OF(ptr, type, member) ((type *)((char *)(ptr) - offsetof(type, member)))

namespace hostsl {

    namespace {

        // SL result codes (from SLresult's 1.0.1 table).
        const u32 HOST_SL_RESULT_SUCCESS = 0x00000000;
        const u32 HOST_SL_RESULT_PARAMETER_INVALID = 0x0000000b;
        const u32 HOST_SL_RESULT_BUFFER_INSUFFICIENT = 0x0000000c;
        const u32 HOST_SL_RESULT_CONTENT_UNSUPPORTED = 0x0000000e;

        // SL object kinds (fake-internal dispatch for the shared ObjectItf).
        const u32 HOST_KIND_ENGINE = 1;
        const u32 HOST_KIND_MIX = 2;
        const u32 HOST_KIND_PLAYER = 3;

        // The device sink: everything is converted to 16-bit stereo.
        SDL_AudioSpec host_device_spec = {SDL_AUDIO_S16LE, 2, 48000};
        SDL_AudioStream *host_device_stream = NULL;

        struct HostStats host_stats = {};

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

        enum { HOST_PLAYER_MAX_QUEUE = 16 };

        // One SL buffer-queue entry. The game owns the buffer memory it passes
        // to Enqueue (the decoder ring reuses it for the next chunk), so the
        // device keeps its own copy until the entry is consumed.
        struct QueueEntry {
            u8 *data;        // the device copy (stream source format)
            u32 stream_size; // the copy size in the stream source format
            u32 fed;         // bytes already handed to the device-side buffer
            u32 sl_size;     // the size the game enqueued (SL accounting)
        };

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

            // device-side consumption bookkeeping (the playhead: frames pulled
            // out of the device-side buffer)
            u64 consumed_device_frames;
            // the SL buffer queue, consumed in order
            QueueEntry queue[HOST_PLAYER_MAX_QUEUE];
            u32 queue_head;
            u32 queue_count;

            // underrun tracking (the HEADATEND condition: playing, queue empty)
            bool underrunning;
            u64 underrun_start_ms;
        };

        // ---------------------------------------------------------------------------
        // static vtables
        // ---------------------------------------------------------------------------

        // ObjectItf (shared by engine / mix / player)
        u32 host_object_realize(void *self, u32 async);
        u32 host_object_resume(void *self);
        u32 host_object_get_state(void *self, u32 *out);
        u32 host_object_get_interface(void *self, const void *iid, void **out);
        u32 host_object_destroy(void *self);

        // EngineItf / EngineCapabilitiesItf
        u32 host_engine_create_audio_player(void *self, void **player_object, void *audio_src, void *audio_sink,
                                            u32 num_interfaces, const void **interface_ids, const u32 *required);
        u32 host_engine_create_output_mix(void *self, void **mix_object, u32 num_interfaces, const void **interface_ids,
                                          const u32 *required);
        u32 host_engine_query_supported_profiles(void *self, u16 *profiles);
        u32 host_engine_query_available_outputs(void *self, u32 max_outputs, u32 *output_ids, u32 *output_details,
                                                u32 *num_outputs);
        u32 host_engine_query_realtime(void *self, u32 *config);

        // PlayItf
        u32 host_play_set_play_state(void *self, u32 state);
        u32 host_play_get_play_state(void *self, u32 *state);
        u32 host_play_get_position(void *self, u32 *millisec);
        u32 host_play_register_callback(void *self, PlayCallbackFn callback, void *context);
        u32 host_play_set_callback_events_mask(void *self, u32 mask);

        // AndroidSimpleBufferQueueItf
        u32 host_queue_enqueue(void *self, void *data, u32 size);
        u32 host_queue_clear(void *self);
        u32 host_queue_get_state(void *self, u32 *count);

        // VolumeItf
        u32 host_volume_set_volume_level(void *self, i32 level);
        u32 host_volume_enable_stereo_position(void *self, u32 enabled);
        u32 host_volume_set_stereo_position(void *self, i32 position);

        // device mix loop
        void host_device_mix_callback(void *userdata, SDL_AudioStream *stream, int additional, int total);

        const ObjectVTable host_object_vt = {
            host_object_realize, host_object_resume, host_object_get_state, host_object_get_interface, NULL, NULL,
            host_object_destroy,
        };
        const EngineVTable host_engine_vt = {
            NULL, NULL, host_engine_create_audio_player, NULL, NULL, NULL, NULL, host_engine_create_output_mix,
        };
        const EngineCapsVTable host_caps_vt = {
            host_engine_query_supported_profiles,
            host_engine_query_available_outputs,
            host_engine_query_realtime,
        };
        const PlayVTable host_play_vt = {
            NULL,
            host_play_set_play_state,
            host_play_get_play_state,
            host_play_get_position,
            host_play_register_callback,
            host_play_set_callback_events_mask,
        };
        const QueueVTable host_queue_vt = {
            host_queue_enqueue,
            host_queue_clear,
            host_queue_get_state,
        };
        const VolumeVTable host_volume_vt = {
            NULL, NULL,
            NULL, host_volume_set_volume_level,
            NULL, host_volume_enable_stereo_position,
            NULL, host_volume_set_stereo_position,
        };

        // ---------------------------------------------------------------------------
        // shared registry (device callback thread vs NuMain thread)
        // ---------------------------------------------------------------------------

        // The device-side buffer (the AudioTrack analog): real SL copies the
        // SL queue's front buffer into the AudioTrack as its buffer drains and
        // removes buffers from the SL queue on that hand-off, so the playhead
        // lags the queue by the AudioTrack buffer size. The cap bounds how much the
        // device holds; 16384 device bytes ≈ 4 × 21 ms periods at 48 kHz.
        const u32 HOST_TRACK_CAP_BYTES = 16384;

        // Frees and drops every queued entry.
        void host_queue_release_all(Player *player) {
            while (player->queue_count > 0) {
                QueueEntry *entry = &player->queue[player->queue_head];
                free(entry->data);
                entry->data = NULL;
                player->queue_head = (player->queue_head + 1) % HOST_PLAYER_MAX_QUEUE;
                player->queue_count--;
            }
        }

        pthread_mutex_t host_lock = PTHREAD_MUTEX_INITIALIZER;
        Player *host_players[32];
        u32 host_player_count = 0;

        void host_register_player(Player *player) {
            pthread_mutex_lock(&host_lock);
            if (host_player_count < 32) {
                host_players[host_player_count++] = player;
                host_stats.players_created++;
            }
            pthread_mutex_unlock(&host_lock);
        }

        void host_remove_player(Player *player) {
            pthread_mutex_lock(&host_lock);
            for (u32 i = 0; i < host_player_count; i++) {
                if (host_players[i] == player) {
                    host_players[i] = host_players[--host_player_count];
                    break;
                }
            }
            pthread_mutex_unlock(&host_lock);
            host_queue_release_all(player);
            if (player->stream != NULL) {
                SDL_DestroyAudioStream(player->stream);
                player->stream = NULL;
            }
        }

        // ---------------------------------------------------------------------------
        // ObjectItf (shared by engine / mix / player)
        // ---------------------------------------------------------------------------

        u32 host_object_realize(void *self, u32 async) {
            (void)async;
            switch (((ObjectVTable **)self)[0] == &host_object_vt ? ((u32 *)self)[1] : 0) {
                case HOST_KIND_ENGINE:
                    ((EngineObject *)self)->realized = true;
                    return HOST_SL_RESULT_SUCCESS;
                case HOST_KIND_MIX:
                    ((MixObject *)self)->realized = true;
                    return HOST_SL_RESULT_SUCCESS;
                case HOST_KIND_PLAYER:
                    ((Player *)self)->realized = true;
                    return HOST_SL_RESULT_SUCCESS;
                default:
                    return HOST_SL_RESULT_PARAMETER_INVALID;
            }
        }

        // SLObjectItf::Resume (object vtable slot 0x4). The engine's state poll uses
        // it to resume a suspended player object.
        u32 host_object_resume(void *self) {
            (void)self;
            return HOST_SL_RESULT_SUCCESS;
        }

        // SLObjectItf::GetState (object vtable slot 0x8): the realized state of the
        // object, not the play state.
        u32 host_object_get_state(void *self, u32 *out) {
            const u32 kind = ((u32 *)self)[1];
            bool realized = false;
            if (kind == HOST_KIND_ENGINE) {
                realized = ((EngineObject *)self)->realized;
            } else if (kind == HOST_KIND_MIX) {
                realized = ((MixObject *)self)->realized;
            } else if (kind == HOST_KIND_PLAYER) {
                realized = ((Player *)self)->realized;
            }

            // OpenSL ES 1.0.1: UNREALIZED=1, REALIZED=2, SUSPENDED=3.
            // Returning 1 for an already-realized player sent the decompiled
            // UpdateState down its re-realization recovery path during the
            // priming Update in Play(). The real object reports 2 here.
            *out = realized ? 2 : 1;
            return HOST_SL_RESULT_SUCCESS;
        }

        u32 host_object_get_interface(void *self, const void *iid, void **out) {
            *out = NULL;
            const u32 kind = ((u32 *)self)[1];

            if (kind == HOST_KIND_ENGINE) {
                EngineObject *engine = (EngineObject *)self;
                if (iid == SL_IID_ENGINE) {
                    *out = &engine->engine_itf;
                    return HOST_SL_RESULT_SUCCESS;
                }
                if (iid == SL_IID_ENGINECAPABILITIES) {
                    *out = &engine->caps_itf;
                    return HOST_SL_RESULT_SUCCESS;
                }
                return HOST_SL_RESULT_PARAMETER_INVALID;
            }

            if (kind == HOST_KIND_PLAYER) {
                Player *player = (Player *)self;
                if (iid == SL_IID_PLAY) {
                    *out = &player->play_itf;
                    return HOST_SL_RESULT_SUCCESS;
                }
                if (iid == SL_IID_ANDROIDSIMPLEBUFFERQUEUE) {
                    *out = &player->queue_itf;
                    return HOST_SL_RESULT_SUCCESS;
                }
                if (iid == SL_IID_VOLUME) {
                    *out = &player->volume_itf;
                    return HOST_SL_RESULT_SUCCESS;
                }
                return HOST_SL_RESULT_PARAMETER_INVALID;
            }

            // Output mix: the original tolerates both of its interface queries
            // failing (nothing in the transcribed scope reads the results).
            return HOST_SL_RESULT_PARAMETER_INVALID;
        }

        u32 host_object_destroy(void *self) {
            const u32 kind = ((u32 *)self)[1];
            if (kind == HOST_KIND_PLAYER) {
                host_remove_player((Player *)self);
                free(self);
                return HOST_SL_RESULT_SUCCESS;
            }
            if (kind == HOST_KIND_MIX) {
                free(self);
                return HOST_SL_RESULT_SUCCESS;
            }
            if (kind == HOST_KIND_ENGINE) {
                // Engine teardown shuts the device.
                if (host_device_stream != NULL) {
                    SDL_DestroyAudioStream(host_device_stream);
                    host_device_stream = NULL;
                }
                free(self);
                return HOST_SL_RESULT_SUCCESS;
            }
            return HOST_SL_RESULT_PARAMETER_INVALID;
        }

        // ---------------------------------------------------------------------------
        // EngineItf / EngineCapabilitiesItf
        // ---------------------------------------------------------------------------

        u32 host_engine_create_audio_player(void *self, void **player_object, void *audio_src, void *audio_sink,
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
                return HOST_SL_RESULT_PARAMETER_INVALID;
            }
            if (pcm[0] != 2) { // SL_DATAFORMAT_PCM
                return HOST_SL_RESULT_CONTENT_UNSUPPORTED;
            }

            const u32 channels = pcm[1];
            const u32 rate = pcm[2] / 1000; // milli Hz -> Hz
            const u32 bits = pcm[3];

            if (channels != 1 && channels != 2) {
                return HOST_SL_RESULT_CONTENT_UNSUPPORTED;
            }
            if (bits != 8 && bits != 16 && bits != 24) {
                return HOST_SL_RESULT_CONTENT_UNSUPPORTED;
            }
            if (rate == 0) {
                return HOST_SL_RESULT_CONTENT_UNSUPPORTED;
            }

            Player *player = (Player *)calloc(1, sizeof(Player));
            if (player == NULL) {
                return HOST_SL_RESULT_PARAMETER_INVALID;
            }

            player->object_vt = (ObjectVTable *)&host_object_vt;
            player->kind = HOST_KIND_PLAYER;
            player->play_itf.vt = (PlayVTable *)&host_play_vt;
            player->queue_itf.vt = (QueueVTable *)&host_queue_vt;
            player->volume_itf.vt = (VolumeVTable *)&host_volume_vt;
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
            player->stream = SDL_CreateAudioStream(&src, &host_device_spec);

            host_register_player(player);
            *player_object = player;
            return HOST_SL_RESULT_SUCCESS;
        }

        u32 host_engine_create_output_mix(void *self, void **mix_object, u32 num_interfaces, const void **interface_ids,
                                          const u32 *required) {
            (void)self;
            (void)num_interfaces;
            (void)interface_ids;
            (void)required;

            // The output mix IS the SDL device: open it on first use. Real SL
            // mixes in ~21 ms callbacks; SDL's default initial device buffer is
            // far larger, which lets one mix pass consume the whole queued
            // buffer and breaks the real-time consumption cadence the game's
            // streaming refill depends on.
            if (host_device_stream == NULL) {
                SDL_SetHint(SDL_HINT_AUDIO_DEVICE_SAMPLE_FRAMES, "1024");
                host_device_stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &host_device_spec,
                                                               host_device_mix_callback, NULL);
                if (host_device_stream == NULL) {
                    *mix_object = NULL;
                    return HOST_SL_RESULT_PARAMETER_INVALID;
                }
                SDL_ResumeAudioStreamDevice(host_device_stream);
            }

            MixObject *mix = (MixObject *)calloc(1, sizeof(MixObject));
            if (mix == NULL) {
                return HOST_SL_RESULT_PARAMETER_INVALID;
            }
            mix->object_vt = (ObjectVTable *)&host_object_vt;
            mix->kind = HOST_KIND_MIX;
            *mix_object = mix;
            return HOST_SL_RESULT_SUCCESS;
        }

        u32 host_engine_query_supported_profiles(void *self, u16 *profiles) {
            (void)self;
            // Advertise no optional profiles: the original skips its optional output
            // queries unless the android profile bit (0x4) is set.
            *profiles = 0;
            return HOST_SL_RESULT_SUCCESS;
        }

        u32 host_engine_query_available_outputs(void *self, u32 max_outputs, u32 *output_ids, u32 *output_details,
                                                u32 *num_outputs) {
            (void)self;
            (void)max_outputs;
            (void)output_ids;
            (void)output_details;
            *num_outputs = 0;
            return HOST_SL_RESULT_SUCCESS;
        }

        u32 host_engine_query_realtime(void *self, u32 *config) {
            (void)self;
            *config = 0;
            return HOST_SL_RESULT_SUCCESS;
        }

        // ---------------------------------------------------------------------------
        // PlayItf
        // ---------------------------------------------------------------------------

        u32 host_play_set_play_state(void *self, u32 state) {
            Player *player = (Player *)HOSTSL_CONTAINER_OF(self, Player, play_itf);
            player->play_state = state;
            return HOST_SL_RESULT_SUCCESS;
        }

        u32 host_play_get_play_state(void *self, u32 *state) {
            *state = ((Player *)HOSTSL_CONTAINER_OF(self, Player, play_itf))->play_state;
            return HOST_SL_RESULT_SUCCESS;
        }

        u32 host_play_get_position(void *self, u32 *millisec) {
            Player *player = (Player *)HOSTSL_CONTAINER_OF(self, Player, play_itf);
            const u64 consumed_src_frames =
                player->consumed_device_frames * (u64)player->rate / (u64)host_device_spec.freq;
            *millisec = (u32)(consumed_src_frames * 1000ULL / (u64)player->rate);
            return HOST_SL_RESULT_SUCCESS;
        }

        u32 host_play_register_callback(void *self, PlayCallbackFn callback, void *context) {
            Player *player = (Player *)HOSTSL_CONTAINER_OF(self, Player, play_itf);
            player->play_callback = callback;
            player->callback_context = context;
            return HOST_SL_RESULT_SUCCESS;
        }

        u32 host_play_set_callback_events_mask(void *self, u32 mask) {
            ((Player *)HOSTSL_CONTAINER_OF(self, Player, play_itf))->callback_mask = mask;
            return HOST_SL_RESULT_SUCCESS;
        }

        // ---------------------------------------------------------------------------
        // AndroidSimpleBufferQueueItf
        // ---------------------------------------------------------------------------

        u32 host_queue_enqueue(void *self, void *data, u32 size) {
            Player *player = (Player *)HOSTSL_CONTAINER_OF(self, Player, queue_itf);
            if (data == NULL || size == 0 || player->stream == NULL) {
                return HOST_SL_RESULT_PARAMETER_INVALID;
            }

            pthread_mutex_lock(&host_lock);
            if (player->queue_count == HOST_PLAYER_MAX_QUEUE) {
                pthread_mutex_unlock(&host_lock);
                return HOST_SL_RESULT_BUFFER_INSUFFICIENT;
            }

            QueueEntry *entry = &player->queue[(player->queue_head + player->queue_count) % HOST_PLAYER_MAX_QUEUE];
            memset(entry, 0, sizeof(*entry));
            entry->sl_size = size;
            if (player->bits == 24) {
                // 24-bit LE samples expand into 32-bit words for SDL.
                const u32 samples = size / 3;
                if (samples > 0x20000) {
                    pthread_mutex_unlock(&host_lock);
                    return HOST_SL_RESULT_BUFFER_INSUFFICIENT;
                }
                entry->data = (u8 *)malloc(samples * 4);
                if (entry->data == NULL) {
                    pthread_mutex_unlock(&host_lock);
                    return HOST_SL_RESULT_BUFFER_INSUFFICIENT;
                }
                const u8 *in = (const u8 *)data;
                u32 *out = (u32 *)entry->data;
                for (u32 i = 0; i < samples; i++) {
                    const u32 value = (u32)in[i * 3 + 0] | ((u32)in[i * 3 + 1] << 8) | ((u32)in[i * 3 + 2] << 16);
                    out[i] = value << 8;
                }
                entry->stream_size = samples * 4;
            } else {
                entry->data = (u8 *)malloc(size);
                if (entry->data == NULL) {
                    pthread_mutex_unlock(&host_lock);
                    return HOST_SL_RESULT_BUFFER_INSUFFICIENT;
                }
                memcpy(entry->data, data, size);
                entry->stream_size = size;
            }

            player->queue_count++;
            host_stats.bytes_enqueued += size;
            pthread_mutex_unlock(&host_lock);
            return HOST_SL_RESULT_SUCCESS;
        }

        u32 host_queue_clear(void *self) {
            Player *player = (Player *)HOSTSL_CONTAINER_OF(self, Player, queue_itf);
            pthread_mutex_lock(&host_lock);
            if (player->stream != NULL) {
                SDL_ClearAudioStream(player->stream);
            }
            host_queue_release_all(player);
            pthread_mutex_unlock(&host_lock);
            return HOST_SL_RESULT_SUCCESS;
        }

        // Drops queue entries that have been handed completely to the
        // device-side buffer; returns the hand-off count for queue accounting.
        u32 host_queue_count_handed_off(Player *player) {
            u32 done = 0;
            while (player->queue_count > 0) {
                QueueEntry *entry = &player->queue[player->queue_head];
                if (entry->fed != entry->stream_size) {
                    break;
                }
                free(entry->data);
                entry->data = NULL;
                player->queue_head = (player->queue_head + 1) % HOST_PLAYER_MAX_QUEUE;
                player->queue_count--;
                done++;
            }
            return done;
        }

        u32 host_queue_get_state(void *self, u32 *count) {
            Player *player = (Player *)HOSTSL_CONTAINER_OF(self, Player, queue_itf);
            pthread_mutex_lock(&host_lock);
            host_queue_count_handed_off(player);
            *count = player->queue_count;
            pthread_mutex_unlock(&host_lock);
            return HOST_SL_RESULT_SUCCESS;
        }

        // ---------------------------------------------------------------------------
        // VolumeItf
        // ---------------------------------------------------------------------------

        u32 host_volume_set_volume_level(void *self, i32 level) {
            ((Player *)HOSTSL_CONTAINER_OF(self, Player, volume_itf))->volume_millibels = level;
            return HOST_SL_RESULT_SUCCESS;
        }

        u32 host_volume_enable_stereo_position(void *self, u32 enabled) {
            ((Player *)HOSTSL_CONTAINER_OF(self, Player, volume_itf))->stereo_position_enabled = enabled != 0;
            return HOST_SL_RESULT_SUCCESS;
        }

        u32 host_volume_set_stereo_position(void *self, i32 position) {
            Player *player = (Player *)HOSTSL_CONTAINER_OF(self, Player, volume_itf);
            if (position < -1000 || position > 1000) {
                return HOST_SL_RESULT_PARAMETER_INVALID;
            }
            player->stereo_position_milli = position;
            return HOST_SL_RESULT_SUCCESS;
        }

        // ---------------------------------------------------------------------------
        // device mix loop
        // ---------------------------------------------------------------------------

        const u32 HOST_MIX_CHUNK_BYTES = 4096; // one pull per player per pass
        enum { HOST_FIRED_MAX = 64 };

        // Hands queue data to the device-side buffer while it has room (the
        // AudioTrack analog). Finishing an SL queue entry is not a play-interface
        // event: SL_PLAYEVENT_HEADATEND fires only when the play head itself
        // reaches the end of all queued audio.
        void host_top_up_player_stream(Player *player) {
            host_queue_count_handed_off(player);

            const u32 available = (u32)SDL_GetAudioStreamAvailable(player->stream);
            u32 space = HOST_TRACK_CAP_BYTES > available ? HOST_TRACK_CAP_BYTES - available : 0;
            space &= ~3u; // whole s16 stereo frames in the device format
            while (space > 0 && player->queue_count > 0) {
                QueueEntry *entry = &player->queue[player->queue_head];
                const u32 remaining = entry->stream_size - entry->fed;
                const u32 take = remaining < space ? remaining : space;
                if (!SDL_PutAudioStreamData(player->stream, entry->data + entry->fed, (int)take)) {
                    break;
                }
                entry->fed += take;
                space -= take;
                host_queue_count_handed_off(player);
            }
        }

        void host_device_mix_callback(void *userdata, SDL_AudioStream *stream, int additional, int total) {
            (void)userdata;
            (void)total;

            static u8 host_pull_buffer[HOST_MIX_CHUNK_BYTES];
            static f32 host_mix_acc[HOST_MIX_CHUNK_BYTES / 2]; // s16 stereo frame accumulator

            // Buffer-consumed events fire after the registry lock is dropped: the
            // handler takes the voice mutex, and Enqueue holds that mutex while
            // touching the registry, so taking it under host_lock would invert the order.
            Player *fired[HOST_FIRED_MAX];
            u32 fired_count = 0;

            pthread_mutex_lock(&host_lock);

            while (additional > 0) {
                u32 chunk = (additional < (int)HOST_MIX_CHUNK_BYTES) ? (u32)additional : HOST_MIX_CHUNK_BYTES;
                chunk &= ~3u; // whole s16 stereo frames
                if (chunk == 0) {
                    break;
                }
                const u32 frames = chunk / 4;

                memset(host_mix_acc, 0, (usize)chunk * 2);

                for (u32 i = 0; i < host_player_count; i++) {
                    Player *player = host_players[i];
                    if (player->play_state != 3 || player->stream == NULL) {
                        continue; // stopped/paused players keep their data queued
                    }

                    // The device-side buffer drains into the mix; the SL queue
                    // keeps feeding it while there is room. Play-interface events
                    // are based on the play head below, not this queue hand-off.
                    host_top_up_player_stream(player);

                    const u32 available = (u32)SDL_GetAudioStreamAvailable(player->stream);
                    const u32 take = (available < chunk ? available : chunk) & ~3u;
                    if (take == 0) {
                        // Underrun: nothing queued means the play head sits at the end
                        // of the last buffer, which is when real SL raises
                        // SL_PLAYEVENT_HEADATEND. The streaming refill starts here.
                        if (!player->underrunning) {
                            player->underrunning = true;
                            player->underrun_start_ms = SDL_GetTicks();
                            LOG_WARN("audio: buffer underrun on player %p (playing, queue empty)", (void *)player);
                            if (fired_count < HOST_FIRED_MAX) {
                                fired[fired_count++] = player;
                            }
                        }
                        continue;
                    }
                    const int got = SDL_GetAudioStreamData(player->stream, host_pull_buffer, (int)take);
                    if (got < 4) {
                        continue;
                    }
                    if (player->underrunning) {
                        player->underrunning = false;
                        LOG_WARN("audio: player %p underrun recovered after %.1f ms", (void *)player,
                                 (double)(SDL_GetTicks() - player->underrun_start_ms));
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

                    const i16 *in = (const i16 *)host_pull_buffer;
                    for (u32 f = 0; f < (u32)got / 4; f++) {
                        host_mix_acc[f * 2 + 0] += (f32)in[f * 2 + 0] * left;
                        host_mix_acc[f * 2 + 1] += (f32)in[f * 2 + 1] * right;
                    }

                    player->consumed_device_frames += (u32)got / 4;
                    host_stats.bytes_consumed += (u32)got;
                }

                i16 *out16 = (i16 *)host_pull_buffer;
                f64 sum = 0;
                for (u32 f = 0; f < frames * 2; f++) {
                    f32 sample = host_mix_acc[f];
                    if (sample > 32767.0f) {
                        sample = 32767.0f;
                    } else if (sample < -32768.0f) {
                        sample = -32768.0f;
                    }
                    out16[f] = (i16)sample;
                    sum += (f64)out16[f] * out16[f];
                }
                host_stats.last_mix_rms = (f32)sqrt(sum / (frames * 2));

                SDL_PutAudioStreamData(stream, out16, (int)chunk);
                additional -= (int)chunk;
            }

            pthread_mutex_unlock(&host_lock);

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

    u32 host_create_engine(void **engine_object, u32 num_options, const void *options, u32 num_interfaces,
                           const void **interface_ids, const u32 *required) {
        (void)num_options;
        (void)options;
        (void)num_interfaces;
        (void)interface_ids;
        (void)required;

        *engine_object = NULL;
        EngineObject *engine = (EngineObject *)calloc(1, sizeof(EngineObject));
        if (engine == NULL) {
            return HOST_SL_RESULT_PARAMETER_INVALID;
        }
        engine->object_vt = (ObjectVTable *)&host_object_vt;
        engine->kind = HOST_KIND_ENGINE;
        engine->engine_itf.vt = (EngineVTable *)&host_engine_vt;
        engine->caps_itf.vt = (EngineCapsVTable *)&host_caps_vt;
        *engine_object = engine;
        return HOST_SL_RESULT_SUCCESS;
    }

    HostStats HostGetStats() {
        pthread_mutex_lock(&host_lock);
        HostStats stats = host_stats;
        stats.players_playing = 0;
        for (u32 i = 0; i < host_player_count; i++) {
            if (host_players[i]->play_state == 3) {
                stats.players_playing++;
            }
        }
        pthread_mutex_unlock(&host_lock);
        return stats;
    }

} // namespace hostsl

namespace {
    char host_sl_iid_play;
    char host_sl_iid_buffer_queue;
    char host_sl_iid_volume;
    char host_sl_iid_engine;
    char host_sl_iid_engine_capabilities;
    char host_sl_iid_environmental_reverb;
} // namespace

extern "C" {
    const void *SL_IID_PLAY = &host_sl_iid_play;
    const void *SL_IID_ANDROIDSIMPLEBUFFERQUEUE = &host_sl_iid_buffer_queue;
    const void *SL_IID_VOLUME = &host_sl_iid_volume;
    const void *SL_IID_ENGINE = &host_sl_iid_engine;
    const void *SL_IID_ENGINECAPABILITIES = &host_sl_iid_engine_capabilities;
    const void *SL_IID_ENVIRONMENTALREVERB = &host_sl_iid_environmental_reverb;

    u32 slCreateEngine(void **engine_object, u32 num_options, const void *options, u32 num_interfaces,
                       const void **interface_ids, const u32 *required) {
        return hostsl::host_create_engine(engine_object, num_options, options, num_interfaces, interface_ids, required);
    }
}
