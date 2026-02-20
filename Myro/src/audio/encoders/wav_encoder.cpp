#include "wav_encoder.h"

#include <cstring>
#include "../detail/detail.h"

namespace myro
{
    bool wav_encoder::init(const std::filesystem::path& output_filepath, unsigned int sample_rate, unsigned int channels)
    {
        m_sample_rate = sample_rate;
        m_channels = static_cast<uint16_t>(channels);
        m_out_file = detail::open_file(output_filepath, "wb");
        if (!m_out_file) 
            return false;

        write_header_placeholder();
        return true;
    }

    void wav_encoder::deinit()
    {
        if (!m_out_file) 
            return;
        update_header();
        detail::fclose_checked(m_out_file);
        m_out_file = nullptr;
    }

    bool wav_encoder::initialized() const
    {
        return static_cast<bool>(m_out_file);
    }

    void wav_encoder::write(const short* pcm_frames, size_t frame_count)
    {
        if (!m_out_file) return;
        size_t bytes_to_write = frame_count * m_channels * (m_bits_per_sample / 8);
        detail::fwrite_checked(pcm_frames, 1, bytes_to_write, m_out_file);
        m_data_bytes_written += static_cast<uint32_t>(bytes_to_write);
    }

    void wav_encoder::write_header_placeholder()
	{
        std::memcpy(m_header, "RIFF\0\0\0\0WAVEfmt \x10\0\0\0\x01\0\0\0\0\0\0\0\0\0\0\0\x02\0\x10\0data\0\0\0\0", 44);
        *reinterpret_cast<uint16_t*>(m_header + 22) = m_channels;
        *reinterpret_cast<uint32_t*>(m_header + 24) = m_sample_rate;
        *reinterpret_cast<uint32_t*>(m_header + 28) = m_sample_rate * m_channels * (m_bits_per_sample / 8);
        *reinterpret_cast<uint16_t*>(m_header + 32) = m_channels * (m_bits_per_sample / 8);
        detail::fwrite_checked(m_header, 1, 44, m_out_file);
	}

    void wav_encoder::update_header()
    {
        if (!m_out_file) 
            return;

        uint32_t chunkSize = 36 + m_data_bytes_written;
        *reinterpret_cast<uint32_t*>(m_header + 4) = chunkSize;
        *reinterpret_cast<uint32_t*>(m_header + 40) = m_data_bytes_written;
        detail::fseek_checked(m_out_file, 0, SEEK_SET);
        detail::fwrite_checked(m_header, 1, 44, m_out_file);
    }
}
