#pragma once

#include <filesystem>

namespace myro
{
    class Iencoder
    {
    public:
        virtual ~Iencoder() = default;

        virtual bool init(const std::filesystem::path& output_filepath, unsigned int sample_rate, unsigned int channels) = 0;
        virtual void uninit() = 0;

        virtual bool initialized() const = 0;

        virtual uint32_t get_sample_rate() const = 0;
        virtual uint16_t get_channels() const = 0;
        virtual uint16_t get_bits_per_sample() const = 0;

        operator bool() const 
        { 
            return initialized(); 
        }
    protected:
        virtual void write(const short* pcm_frames, size_t frame_count) = 0;

        friend class audio_capture;
    };
}