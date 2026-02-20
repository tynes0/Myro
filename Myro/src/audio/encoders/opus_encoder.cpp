#include "audio/encoders/opus_encoder.h"
#include "core/log.h"

#include <vector>
#include <cstring>
#include <fstream>
#include <random>

#include <opus.h>
#include <ogg/ogg.h>

namespace myro
{
    namespace
    {
        void write_le16(unsigned char* ptr, uint16_t val)
        {
            ptr[0] = val & 0xFF; ptr[1] = (val >> 8) & 0xFF;
        }
        
        void write_le32(unsigned char* ptr, uint32_t val)
        {
            ptr[0] = val & 0xFF; ptr[1] = (val >> 8) & 0xFF;
            ptr[2] = (val >> 16) & 0xFF; ptr[3] = (val >> 24) & 0xFF;
        }
        
    }

    struct _opus_encoder_data
    {
        std::ofstream file;
        
        bool initialized = false;
        uint32_t sample_rate = 0;
        uint16_t channels = 0;
        
        OpusEncoder* encoder = nullptr;
        
        ogg_stream_state os;
        ogg_page         og;
        ogg_packet       op;

        int64_t granule_pos = 0;
        int64_t packet_count = 0;

        std::vector<short> pcm_buffer;
        int frames_per_packet = 0; 
    };

    opus_encoder::opus_encoder() : m_data(new _opus_encoder_data{})
    {
    }

    opus_encoder::~opus_encoder()
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

    bool opus_encoder::init(const std::filesystem::path& output_filepath, unsigned int sample_rate, unsigned int channels)
    {
        if (m_data->initialized)
        {
            log::warn("opus_encoder already initialized!");
            return false;
        }
        
        if (sample_rate != 8000 && sample_rate != 12000 && sample_rate != 16000 && sample_rate != 24000 && sample_rate != 48000)
        {
            log::error("Opus encoder requires sample rate to be 8000, 12000, 16000, 24000, or 48000. Got: {}", sample_rate);
            return false;
        }

        m_data->sample_rate = sample_rate;
        m_data->channels = static_cast<uint16_t>(channels);

        int err = OPUS_OK;
        m_data->encoder = opus_encoder_create(static_cast<opus_int32>(m_data->sample_rate), m_data->channels, OPUS_APPLICATION_AUDIO, &err);
        if (err != OPUS_OK)
        {
            log::error("Opus encoder could not be created. Error code: {}", err);
            return false;
        }

        m_data->frames_per_packet = static_cast<int>(m_data->sample_rate) / 50;

        m_data->file.open(output_filepath, std::ios::binary);
        if (!m_data->file.is_open())
        {
            log::error("File could not be created for opus encoder: {}", output_filepath.string());
            opus_encoder_destroy(m_data->encoder);
            return false;
        }

        ogg_stream_init(&m_data->os, static_cast<int>(std::random_device{}()));

        unsigned char header_data[19];
        std::memcpy(header_data, "OpusHead", 8);
        header_data[8] = 1; // version
        header_data[9] = static_cast<unsigned char>(m_data->channels);
        write_le16(header_data + 10, 384); // Pre-skip
        write_le32(header_data + 12, m_data->sample_rate); // Original Sample Rate
        write_le16(header_data + 16, 0); // Gain
        header_data[18] = 0; // Channel mapping family

        m_data->op.packet = header_data;
        m_data->op.bytes = 19;
        m_data->op.b_o_s = 1;
        m_data->op.e_o_s = 0;
        m_data->op.granulepos = 0;
        m_data->op.packetno = m_data->packet_count++;
        ogg_stream_packetin(&m_data->os, &m_data->op);

        const char* vendor = "Myro Audio Engine";
        uint32_t vendor_len = static_cast<uint32_t>(std::strlen(vendor));
        std::vector<unsigned char> tags_data(8 + 4 + vendor_len + 4);
        
        std::memcpy(tags_data.data(), "OpusTags", 8);
        write_le32(tags_data.data() + 8, vendor_len);
        std::memcpy(tags_data.data() + 12, vendor, vendor_len);
        write_le32(tags_data.data() + 12 + vendor_len, 0);

        m_data->op.packet = tags_data.data();
        m_data->op.bytes = static_cast<long>(tags_data.size());
        m_data->op.b_o_s = 0;
        m_data->op.packetno = m_data->packet_count++;
        ogg_stream_packetin(&m_data->os, &m_data->op);

        flush_ogg_pages(true);

        m_data->initialized = true;
        return true;
    }

    void opus_encoder::deinit()
    {
        deinit_impl();
    }

    bool opus_encoder::initialized() const { return m_data->initialized; }
    uint32_t opus_encoder::get_sample_rate() const { return m_data->sample_rate; }
    uint16_t opus_encoder::get_channels() const { return m_data->channels; }
    uint16_t opus_encoder::get_bits_per_sample() const { return 16; }

    void opus_encoder::write(const short* pcm_frames, size_t frame_count)
    {
        if (!m_data->initialized || frame_count == 0) return;

        m_data->pcm_buffer.insert(m_data->pcm_buffer.end(), pcm_frames, pcm_frames + (frame_count * m_data->channels));

        int samples_per_packet = m_data->frames_per_packet * m_data->channels;

        while (m_data->pcm_buffer.size() >= static_cast<size_t>(samples_per_packet))
        {
            unsigned char out_packet[4000];
            
            int bytes = opus_encode(
                m_data->encoder, 
                m_data->pcm_buffer.data(), 
                m_data->frames_per_packet, 
                out_packet, 
                sizeof(out_packet)
            );

            if (bytes > 0)
            {
                m_data->granule_pos += (m_data->frames_per_packet * 48000) / m_data->sample_rate;

                m_data->op.packet = out_packet;
                m_data->op.bytes = bytes;
                m_data->op.b_o_s = 0;
                m_data->op.e_o_s = 0;
                m_data->op.granulepos = m_data->granule_pos;
                m_data->op.packetno = m_data->packet_count++;

                ogg_stream_packetin(&m_data->os, &m_data->op);
                flush_ogg_pages(false);
            }

            m_data->pcm_buffer.erase(m_data->pcm_buffer.begin(), m_data->pcm_buffer.begin() + samples_per_packet);
        }
    }

    void opus_encoder::flush_ogg_pages(bool force_flush)
    {
        while (true)
        {
            int result = force_flush ? ogg_stream_flush(&m_data->os, &m_data->og) 
                                     : ogg_stream_pageout(&m_data->os, &m_data->og);
            if (result == 0) break;
            
            m_data->file.write(reinterpret_cast<char*>(m_data->og.header), m_data->og.header_len);
            m_data->file.write(reinterpret_cast<char*>(m_data->og.body), m_data->og.body_len);
        }
    }

    void opus_encoder::deinit_impl()
    {
        if (!m_data->initialized) return;

        if (!m_data->pcm_buffer.empty())
        {
            int samples_per_packet = m_data->frames_per_packet * m_data->channels;
            m_data->pcm_buffer.resize(samples_per_packet, 0);

            unsigned char out_packet[4000];
            int bytes = opus_encode(m_data->encoder, m_data->pcm_buffer.data(), m_data->frames_per_packet, out_packet, sizeof(out_packet));
            
            if (bytes > 0)
            {
                m_data->granule_pos += (m_data->frames_per_packet * 48000) / m_data->sample_rate;

                m_data->op.packet = out_packet;
                m_data->op.bytes = bytes;
                m_data->op.b_o_s = 0;
                m_data->op.e_o_s = 1; // END OF STREAM signal!
                m_data->op.granulepos = m_data->granule_pos;
                m_data->op.packetno = m_data->packet_count++;

                ogg_stream_packetin(&m_data->os, &m_data->op);
            }
        }

        flush_ogg_pages(true);

        ogg_stream_clear(&m_data->os);
        
        if (m_data->encoder)
            opus_encoder_destroy(m_data->encoder);

        if (m_data->file.is_open())
            m_data->file.close();

        m_data->initialized = false;
    }
}