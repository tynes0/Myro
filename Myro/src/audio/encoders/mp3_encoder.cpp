#include "audio/encoders/mp3_encoder.h"
#include "core/log.h"

#include <vector>
#include <fstream>
#include <lame.h>

namespace myro
{
    struct _mp3_encoder_data
    {
        std::ofstream file;
        
        bool initialized = false;
        uint32_t sample_rate = 0;
        uint16_t channels = 0;
        
        lame_t lame_client = nullptr;
        
        std::vector<unsigned char> mp3_buffer;
    };

    mp3_encoder::mp3_encoder() : m_data(new _mp3_encoder_data{}) {}

    mp3_encoder::~mp3_encoder()
    {
        if (m_data)
        {
            if (m_data->initialized)
                deinit_impl();
            delete m_data;
            m_data = nullptr;
        }
    }

    bool mp3_encoder::init(const std::filesystem::path& output_filepath, unsigned int sample_rate, unsigned int channels)
    {
        if (m_data->initialized)
        {
            log::warn("mp3_encoder already initialized!");
            return false;
        }

        m_data->sample_rate = sample_rate;
        m_data->channels = static_cast<uint16_t>(channels);

        m_data->lame_client = lame_init();
        if (!m_data->lame_client)
        {
            log::error("LAME engine could not be initialized.");
            return false;
        }

        lame_set_in_samplerate(m_data->lame_client, static_cast<int>(m_data->sample_rate));
        lame_set_num_channels(m_data->lame_client, m_data->channels);
        
        lame_set_VBR(m_data->lame_client, vbr_default);
        
        if (lame_init_params(m_data->lame_client) < 0)
        {
            log::error("LAME parameters could not be applied.");
            lame_close(m_data->lame_client);
            return false;
        }

        m_data->file.open(output_filepath, std::ios::binary);
        if (!m_data->file.is_open())
        {
            log::error("File could not be created for mp3 encoder: {}", output_filepath.string());
            lame_close(m_data->lame_client);
            return false;
        }

        m_data->initialized = true;
        return true;
    }

    void mp3_encoder::deinit() { deinit_impl(); }
    bool mp3_encoder::initialized() const { return m_data->initialized; }
    uint32_t mp3_encoder::get_sample_rate() const { return m_data->sample_rate; }
    uint16_t mp3_encoder::get_channels() const { return m_data->channels; }
    uint16_t mp3_encoder::get_bits_per_sample() const { return 16; }

    void mp3_encoder::write(const short* pcm_frames, size_t frame_count)
    {
        if (!m_data->initialized || frame_count == 0) return;

        // From lame doc: 1.25 * num_samples + 7200
        size_t required_buffer_size = static_cast<size_t>(1.25 * static_cast<size_t>(frame_count) * m_data->channels + 7200);
        if (m_data->mp3_buffer.size() < required_buffer_size)
        {
            m_data->mp3_buffer.resize(required_buffer_size);
        }

        int bytes_written;

        if (m_data->channels == 2)
        {
            bytes_written = lame_encode_buffer_interleaved(
                m_data->lame_client,
                const_cast<short*>(pcm_frames),
                static_cast<int>(frame_count),
                m_data->mp3_buffer.data(),
                static_cast<int>(m_data->mp3_buffer.size())
            );
        }
        else // Mono
        {
            bytes_written = lame_encode_buffer(
                m_data->lame_client,
                const_cast<short*>(pcm_frames), nullptr,
                static_cast<int>(frame_count),
                m_data->mp3_buffer.data(),
                static_cast<int>(m_data->mp3_buffer.size())
            );
        }

        if (bytes_written > 0)
        {
            m_data->file.write(reinterpret_cast<char*>(m_data->mp3_buffer.data()), bytes_written);
        }
        else if (bytes_written < 0)
        {
            log::error("LAME encoding error code: {}", bytes_written);
        }
    }

    void mp3_encoder::deinit_impl()
    {
        if (!m_data->initialized) return;

        int bytes_written = lame_encode_flush(
            m_data->lame_client, 
            m_data->mp3_buffer.data(), 
            static_cast<int>(m_data->mp3_buffer.size())
        );

        if (bytes_written > 0)
        {
            m_data->file.write(reinterpret_cast<char*>(m_data->mp3_buffer.data()), bytes_written);
        }

        lame_close(m_data->lame_client);

        if (m_data->file.is_open())
            m_data->file.close();

        m_data->initialized = false;
    }
}