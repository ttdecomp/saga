#include "nu2api.saga/nusound/nusound_streamer.hpp"

NuList<NuSoundStreamer *> NuSoundStreamer::s_NuSoundStreams;

void NuSoundStreamer::RequestCue(NuSoundStreamingSample *streaming_sample, bool loop, float start_offset,
                                 bool always_false) {

    AddedToThreadQueue();
    // TODO
}
