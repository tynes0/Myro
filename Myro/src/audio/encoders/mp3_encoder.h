#pragma once

#include "iencoder.h"
#include <fstream>
#include <memory>

namespace myro
{
    struct _mp3_encoder_data;
    
    // NOLINTNEXTLINE(cppcoreguidelines-special-member-functions)
    class mp3_encoder : public IEncoder
    {
    public:
        static std::shared_ptr<mp3_encoder> create() { return std::make_shared<mp3_encoder>(); }
        
        mp3_encoder();
        ~mp3_encoder() override;

        bool init(const std::filesystem::path& output_filepath, unsigned int sample_rate, unsigned int channels) override;
        void deinit() override;

        [[nodiscard]] bool initialized() const override;

        [[nodiscard]] uint32_t get_sample_rate() const override;
        [[nodiscard]] uint16_t get_channels() const override;
        [[nodiscard]] uint16_t get_bits_per_sample() const override;

    protected:
        void write(const short* pcm_frames, size_t frame_count) override;

    private:
        void deinit_impl();

        _mp3_encoder_data* m_data = nullptr;
    };
}