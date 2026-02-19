#pragma once

#include <filesystem>

namespace myro
{
    // NOLINTNEXTLINE(cppcoreguidelines-special-member-functions)
    class IEncoder
    {
    public:
        virtual ~IEncoder() = default;

        virtual bool init(const std::filesystem::path& output_filepath, unsigned int sample_rate, unsigned int channels) = 0;
        virtual void deinit() = 0;

        [[nodiscard]] virtual bool initialized() const = 0;

        [[nodiscard]] virtual uint32_t get_sample_rate() const = 0;
        [[nodiscard]] virtual uint16_t get_channels() const = 0;
        [[nodiscard]] virtual uint16_t get_bits_per_sample() const = 0;

        [[nodiscard]] explicit operator bool() const 
        { 
            return initialized(); 
        }
    protected:
        virtual void write(const short* pcm_frames, size_t frame_count) = 0;

        friend class audio_capture;
    };
}