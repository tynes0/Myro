#include "openal_backend.h"

#include "core/log.h"

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 5030)
#endif // _MSC_VER

#include <AL/alext.h>

#ifdef _MSC_VER
#pragma warning(pop)
#endif // _MSC_VER 

namespace myro
{
    struct openal_backend_data
    {
        ALCdevice* audio_device;
    };

    namespace { openal_backend_data s_data; }

    bool openal_backend::init()
    {
        s_data.audio_device = nullptr;
        s_data.audio_device = alcOpenDevice(nullptr);

        if (!s_data.audio_device)
        {
            log::error("Could not open an OpenAL device!");
            return false;
        }

        ALCcontext* context = alcCreateContext(s_data.audio_device, nullptr);
        if (context == nullptr || alcMakeContextCurrent(context) == ALC_FALSE)
        {
            if (context != nullptr)
                alcDestroyContext(context);
            alcCloseDevice(s_data.audio_device);

            log::error("Could not set an OpenAL context!");
            return false;
        }

        return true;
    }

    void openal_backend::shutdown()
    {
        ALCcontext* context = alcGetCurrentContext();
        if (context == nullptr)
            return;

        ALCdevice* device = alcGetContextsDevice(context);

        alcMakeContextCurrent(nullptr);
        alcDestroyContext(context);
        alcCloseDevice(device);
    }

    ALenum openal_backend::get_openAL_format(uint32_t channels, uint32_t bits_per_sample)
    {
        // Note: sample size is always 2 bytes (16-bits) 
        // with the decoders that we're using

        switch (channels)
        {
        case 1: return bits_per_sample == 8 ? AL_FORMAT_MONO8 : AL_FORMAT_MONO16;
        case 2: return bits_per_sample == 8 ? AL_FORMAT_STEREO8 : AL_FORMAT_STEREO16;
        default:
            break;
        }

        MYRO_ASSERT(false, "Unsupported channels!");

        return 0;
    }

    ALCdevice* openal_backend::get_device()
    {
        return s_data.audio_device;
    }
}

