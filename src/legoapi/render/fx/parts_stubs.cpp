#include "legoapi/legoapi_types.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nurand.h"
#include "nu2api/numath/nuvec.h"

#include <float.h>
#include <math.h>
#include <string.h>

extern "C" {
    i32 ParticlesPerFrame(f32 particles_per_frame, f32 frame_time) {
        i32 scaled_count = static_cast<i32>(particles_per_frame * 65536.0f * (frame_time * 60.0f));
        i32 count = 0;

        while (scaled_count > 0xffff) {
            scaled_count -= 0x10000;
            ++count;
        }
        if ((NuRandInt() >> 16) < static_cast<u32>(scaled_count)) {
            ++count;
        }
        return count;
    }

    i32 ParticlesPerSecond(f32 particles_per_second, f32 frame_time) {
        return ParticlesPerFrame(particles_per_second / 60.0f, frame_time);
    }

    void SetPartRTLSet(usize rtl_set) {
        (void)rtl_set;
    }

} // extern "C"

void PartObjectInterface::GetRadius() const {
}

void PartStop_Flickerer(PART_s *) {
}
