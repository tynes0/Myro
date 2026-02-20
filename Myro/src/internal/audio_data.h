#pragma once

#include <cstdint>
#include <AL/al.h>
#include "core/buffer.h"

namespace myro
{
    struct audio_data
    {
        ALenum al_format = 0;
        raw_buffer buffer;
        int64_t sample_rate = 0;
        float track_length = 0.0f;
    };
}