#include "vorbis_encoder.h"
#include "../../core/log.h"

#include <ctime>
#include <random>

#include <vorbis/vorbisenc.h>

namespace myro
{
    struct _vorbis_encoder_data
    {
        std::ofstream file;
        
        bool initialized = false;
        uint32_t sample_rate = 0;
        uint16_t channels = 0;
        
        ogg_stream_state os;
        ogg_page         og;
        ogg_packet       op;

        vorbis_info      vi;
        vorbis_comment   vc;
        vorbis_dsp_state vd;
        vorbis_block     vb;
    };

    vorbis_encoder::vorbis_encoder() : m_data(new _vorbis_encoder_data{})
    {
    }

    vorbis_encoder::~vorbis_encoder()
    {
        if (m_data)
        {
            if (m_data->initialized)
            {
                deinit_impl();
            }
            delete m_data;
            m_data = nullptr;
        }
    }

    bool vorbis_encoder::init(const std::filesystem::path& output_filepath, unsigned int sample_rate, unsigned int channels)
    {
        if (m_data->initialized)
        {
            log::warn("vorbis_encoder already initialized!");
            return false;
        }
        
        m_data->sample_rate = sample_rate;
        m_data->channels = static_cast<uint16_t>(channels);

        m_data->file.open(output_filepath, std::ios::binary);
        if (!m_data->file.is_open())
        {
            log::error("File could not created for vorbis encoder: {}", output_filepath.string());
            return false;
        }

        vorbis_info_init(&m_data->vi);
        int ret = vorbis_encode_init_vbr(&m_data->vi, m_data->channels, static_cast<long>(m_data->sample_rate), 0.4f);
        if (ret != 0)
        {
            log::error("Vorbis encoder could not initialized. (Invalid sample_rate or channels).");
            vorbis_info_clear(&m_data->vi);
            return false;
        }

        vorbis_comment_init(&m_data->vc);
        vorbis_comment_add_tag(&m_data->vc, "ENCODER", "Myro Audio Engine");

        vorbis_analysis_init(&m_data->vd, &m_data->vi);
        vorbis_block_init(&m_data->vd, &m_data->vb);

        ogg_stream_init(&m_data->os, static_cast<int>(std::random_device{}()));

        ogg_packet header, header_comm, header_code;
        vorbis_analysis_headerout(&m_data->vd, &m_data->vc, &header, &header_comm, &header_code);

        ogg_stream_packetin(&m_data->os, &header);
        ogg_stream_packetin(&m_data->os, &header_comm);
        ogg_stream_packetin(&m_data->os, &header_code);

        while (true)
        {
            int result = ogg_stream_flush(&m_data->os, &m_data->og);
            if (result == 0) break;
            m_data->file.write(reinterpret_cast<char*>(m_data->og.header), m_data->og.header_len);
            m_data->file.write(reinterpret_cast<char*>(m_data->og.body), m_data->og.body_len);
        }

        m_data->initialized = true;
        return true;
    }

    void vorbis_encoder::deinit()
    {
        deinit_impl();
    }

    bool vorbis_encoder::initialized() const
    {
        return m_data->initialized;
    }

    uint32_t vorbis_encoder::get_sample_rate() const
    {
        return m_data->sample_rate;
    }

    uint16_t vorbis_encoder::get_channels() const
    {
        return m_data->channels;
    }

    uint16_t vorbis_encoder::get_bits_per_sample() const
    {
        return 16;
    }

    void vorbis_encoder::write(const short* pcm_frames, size_t frame_count)
    {
        if (!m_data->initialized || frame_count == 0) return;

        float** buffer = vorbis_analysis_buffer(&m_data->vd, static_cast<int>(frame_count));

        for (size_t i = 0; i < frame_count; ++i)
        {
            for (uint16_t c = 0; c < m_data->channels; ++c)
            {
                buffer[c][i] = static_cast<float>(pcm_frames[i * m_data->channels + c]) / 32768.f;
            }
        }

        vorbis_analysis_wrote(&m_data->vd, static_cast<int>(frame_count));

        flush_ogg_pages();
    }

    void vorbis_encoder::flush_ogg_pages()
    {
        while (vorbis_analysis_blockout(&m_data->vd, &m_data->vb) == 1)
        {
            vorbis_analysis(&m_data->vb, nullptr);
            vorbis_bitrate_addblock(&m_data->vb);

            while (vorbis_bitrate_flushpacket(&m_data->vd, &m_data->op))
            {
                ogg_stream_packetin(&m_data->os, &m_data->op);

                while (true)
                {
                    int result = ogg_stream_pageout(&m_data->os, &m_data->og);
                    if (result == 0) break;
                    
                    m_data->file.write(reinterpret_cast<char*>(m_data->og.header), m_data->og.header_len);
                    m_data->file.write(reinterpret_cast<char*>(m_data->og.body), m_data->og.body_len);
                }
            }
        }
    }

    void vorbis_encoder::deinit_impl()
    {
        if (!m_data->initialized) return;

        vorbis_analysis_wrote(&m_data->vd, 0);

        flush_ogg_pages();

        ogg_stream_clear(&m_data->os);
        vorbis_block_clear(&m_data->vb);
        vorbis_dsp_clear(&m_data->vd);
        vorbis_comment_clear(&m_data->vc);
        vorbis_info_clear(&m_data->vi);

        if (m_data->file.is_open())
            m_data->file.close();

        m_data->initialized = false;
    }
}
