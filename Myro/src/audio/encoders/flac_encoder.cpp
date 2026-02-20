#include "flac_encoder.h"
#include "../../core/log.h"

#include <FLAC/stream_encoder.h>
#include <vector>

namespace myro
{
    struct _flac_encoder_data
    {
        std::vector<FLAC__int32> interleaved_buffer;
        FLAC__StreamEncoder* flac_encoder = nullptr;
        uint32_t sample_rate = 0;
        uint16_t channels = 0;
        uint16_t bits_per_sample = 16;
        bool is_initialized = false;
    };

    flac_encoder::flac_encoder() : m_data(new _flac_encoder_data())
    {
    }

    flac_encoder::~flac_encoder()
    {
        if (m_data)
        {
            if (m_data->is_initialized)
            {
                deinit_impl();
            }
            delete m_data;
            m_data = nullptr;
        }
    }

    bool flac_encoder::init(const std::filesystem::path& output_filepath, unsigned int sample_rate, unsigned int channels)
    {
        if (m_data->is_initialized)
        {
            log::warn("flac_encoder already initialized!");
            return false;
        }

        m_data->flac_encoder = FLAC__stream_encoder_new();
        if (!m_data->flac_encoder)
            return false;

        FLAC__stream_encoder_set_verify(m_data->flac_encoder, true);
        FLAC__stream_encoder_set_compression_level(m_data->flac_encoder, 5);
        FLAC__stream_encoder_set_channels(m_data->flac_encoder, channels);
        FLAC__stream_encoder_set_bits_per_sample(m_data->flac_encoder, 16);
        FLAC__stream_encoder_set_sample_rate(m_data->flac_encoder, sample_rate);

        m_data->channels = static_cast<uint16_t>(channels);
        m_data->sample_rate = sample_rate;

        auto status = FLAC__stream_encoder_init_file(m_data->flac_encoder, output_filepath.string().c_str(), nullptr, nullptr);

        if (status != FLAC__STREAM_ENCODER_INIT_STATUS_OK)
        {
            FLAC__stream_encoder_delete(m_data->flac_encoder);
            m_data->flac_encoder = nullptr;
            return false;
        }

        m_data->is_initialized = true;
        return true;
    }

    void flac_encoder::deinit()
    {
        deinit_impl();
    }

    bool flac_encoder::initialized() const { return m_data->is_initialized; }
    uint32_t flac_encoder::get_sample_rate() const { return m_data->sample_rate; }
    uint16_t flac_encoder::get_channels() const { return m_data->channels; }
    uint16_t flac_encoder::get_bits_per_sample() const { return m_data->bits_per_sample; }
    
    void flac_encoder::write(const short* pcm_frames, size_t frame_count)
    {
        if (!m_data->is_initialized)
            return;

        m_data->interleaved_buffer.resize(frame_count * m_data->channels);
        for (size_t i = 0; i < m_data->interleaved_buffer.size(); ++i)
            m_data->interleaved_buffer[i] = pcm_frames[i];

        FLAC__stream_encoder_process_interleaved(m_data->flac_encoder, m_data->interleaved_buffer.data(), static_cast<uint32_t>(frame_count));
    }

    void flac_encoder::deinit_impl()
    {
        if (!m_data->is_initialized)
            return;

        FLAC__stream_encoder_finish(m_data->flac_encoder);
        FLAC__stream_encoder_delete(m_data->flac_encoder);
        
        m_data->flac_encoder = nullptr;
        m_data->is_initialized = false;
    }
}
