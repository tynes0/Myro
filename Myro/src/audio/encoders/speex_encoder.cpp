#include "speex_encoder.h"
#include "../../core/log.h"

#include <vector>
#include <cstring>
#include <random>

#include <speex/speex.h>
#include <speex/speex_header.h>
#include <speex/speex_stereo.h>
#include <ogg/ogg.h>

namespace myro
{
    struct _speex_encoder_data
    {
        std::ofstream file;
        
        bool initialized = false;
        uint32_t sample_rate = 0;
        uint16_t channels = 0;
        
        void* speex_state = nullptr;
        SpeexBits bits;
        
        ogg_stream_state os;
        ogg_page         og;
        ogg_packet       op;

        int frame_size = 0;
        int64_t packet_count = 0;
        int64_t granule_pos = 0;

        std::vector<short> pcm_buffer;
    };

    speex_encoder::speex_encoder() : m_data(new _speex_encoder_data{}) {}

    speex_encoder::~speex_encoder()
    {
        if (m_data)
        {
            if (m_data->initialized)
                deinit_impl();
            delete m_data;
            m_data = nullptr;
        }
    }

    bool speex_encoder::init(const std::filesystem::path& output_filepath, unsigned int sample_rate, unsigned int channels)
    {
        if (m_data->initialized)
        {
            log::warn("speex_encoder already initialized!");
            return false;
        }

        m_data->sample_rate = sample_rate;
        m_data->channels = static_cast<uint16_t>(channels);

        const SpeexMode* mode;
        if (sample_rate <= 8000)       mode = speex_lib_get_mode(SPEEX_MODEID_NB);
        else if (sample_rate <= 16000) mode = speex_lib_get_mode(SPEEX_MODEID_WB);
        else                           mode = speex_lib_get_mode(SPEEX_MODEID_UWB);

        m_data->speex_state = speex_encoder_init(mode);
        speex_bits_init(&m_data->bits);

        int quality = 8;
        speex_encoder_ctl(m_data->speex_state, SPEEX_SET_QUALITY, &quality);
        
        int actual_rate = static_cast<int>(sample_rate);
        speex_encoder_ctl(m_data->speex_state, SPEEX_SET_SAMPLING_RATE, &actual_rate);
        speex_encoder_ctl(m_data->speex_state, SPEEX_GET_FRAME_SIZE, &m_data->frame_size);

        m_data->file.open(output_filepath, std::ios::binary);
        if (!m_data->file.is_open())
        {
            log::error("File could not be created for speex encoder: {}", output_filepath.string());
            speex_encoder_destroy(m_data->speex_state);
            speex_bits_destroy(&m_data->bits);
            return false;
        }

        ogg_stream_init(&m_data->os, static_cast<int>(std::random_device{}()));

        SpeexHeader header;
        speex_init_header(&header, static_cast<int>(m_data->sample_rate), m_data->channels, mode);
        
        header.frames_per_packet = 1; 
        header.vbr = 0;

        int header_size = 0;
        char* header_data = speex_header_to_packet(&header, &header_size);

        m_data->op.packet = reinterpret_cast<unsigned char*>(header_data);
        m_data->op.bytes = header_size;
        m_data->op.b_o_s = 1; 
        m_data->op.e_o_s = 0;
        m_data->op.granulepos = 0;
        m_data->op.packetno = m_data->packet_count++;
        ogg_stream_packetin(&m_data->os, &m_data->op);
        
        flush_ogg_pages(true); 
        speex_header_free(header_data);

        const char* comment = "Encoded by Myro Audio Engine";
        int comment_len = static_cast<int>(strlen(comment));
        
        std::vector<unsigned char> comment_packet(8 + comment_len); 
        
        // 4 byte vendor string length (Little Endian)
        uint32_t len = comment_len;
        comment_packet[0] = len & 0xFF; comment_packet[1] = (len >> 8) & 0xFF;
        comment_packet[2] = (len >> 16) & 0xFF; comment_packet[3] = (len >> 24) & 0xFF;
        
        // Vendor string
        std::memcpy(&comment_packet[4], comment, comment_len);
        
        // 4 byte user comment list length = 0
        int offset = 4 + comment_len;
        comment_packet[offset] = 0; comment_packet[offset + 1] = 0;
        comment_packet[offset + 2] = 0; comment_packet[offset + 3] = 0;

        m_data->op.packet = comment_packet.data();
        m_data->op.bytes = static_cast<long>(comment_packet.size());
        m_data->op.b_o_s = 0;
        m_data->op.packetno = m_data->packet_count++;
        ogg_stream_packetin(&m_data->os, &m_data->op);

        flush_ogg_pages(true);

        m_data->initialized = true;
        return true;
    }

    void speex_encoder::deinit() { deinit_impl(); }
    bool speex_encoder::initialized() const { return m_data->initialized; }
    uint32_t speex_encoder::get_sample_rate() const { return m_data->sample_rate; }
    uint16_t speex_encoder::get_channels() const { return m_data->channels; }
    uint16_t speex_encoder::get_bits_per_sample() const { return 16; }

    void speex_encoder::write(const short* pcm_frames, size_t frame_count)
    {
        if (!m_data->initialized || frame_count == 0) return;

        m_data->pcm_buffer.insert(m_data->pcm_buffer.end(), pcm_frames, pcm_frames + (frame_count * m_data->channels));

        int samples_per_frame = m_data->frame_size * m_data->channels;

        while (m_data->pcm_buffer.size() >= static_cast<size_t>(samples_per_frame))
        {
            speex_bits_reset(&m_data->bits);
            std::vector<short> frame_copy(m_data->pcm_buffer.begin(), m_data->pcm_buffer.begin() + samples_per_frame);

            if (m_data->channels == 2)
            {
                speex_encode_stereo_int(frame_copy.data(), m_data->frame_size, &m_data->bits);
                speex_encode_int(m_data->speex_state, frame_copy.data(), &m_data->bits);
            }
            else
            {
                speex_encode_int(m_data->speex_state, frame_copy.data(), &m_data->bits);
            }

            int bytes = speex_bits_nbytes(&m_data->bits);
            std::vector<char> out_packet(bytes);
            speex_bits_write(&m_data->bits, out_packet.data(), bytes);

            m_data->granule_pos += m_data->frame_size;

            m_data->op.packet = reinterpret_cast<unsigned char*>(out_packet.data());
            m_data->op.bytes = bytes;
            m_data->op.b_o_s = 0;
            m_data->op.e_o_s = 0;
            m_data->op.granulepos = m_data->granule_pos;
            m_data->op.packetno = m_data->packet_count++;

            ogg_stream_packetin(&m_data->os, &m_data->op);
            flush_ogg_pages(false);

            m_data->pcm_buffer.erase(m_data->pcm_buffer.begin(), m_data->pcm_buffer.begin() + samples_per_frame);
        }
    }

    void speex_encoder::flush_ogg_pages(bool force_flush)
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

    void speex_encoder::deinit_impl()
    {
        if (!m_data->initialized) return;

        int samples_per_frame = m_data->frame_size * m_data->channels;
        m_data->pcm_buffer.resize(samples_per_frame, 0);

        speex_bits_reset(&m_data->bits);
        std::vector<short> frame_copy(m_data->pcm_buffer.begin(), m_data->pcm_buffer.begin() + samples_per_frame);

        if (m_data->channels == 2)
        {
            speex_encode_stereo_int(frame_copy.data(), m_data->frame_size, &m_data->bits);
            speex_encode_int(m_data->speex_state, frame_copy.data(), &m_data->bits);
        }
        else
        {
            speex_encode_int(m_data->speex_state, frame_copy.data(), &m_data->bits);
        }

        int bytes = speex_bits_nbytes(&m_data->bits);
        std::vector<char> out_packet(bytes);
        speex_bits_write(&m_data->bits, out_packet.data(), bytes);

        m_data->granule_pos += m_data->frame_size;

        m_data->op.packet = reinterpret_cast<unsigned char*>(out_packet.data());
        m_data->op.bytes = bytes;
        m_data->op.b_o_s = 0;
        m_data->op.e_o_s = 1;
        m_data->op.granulepos = m_data->granule_pos;
        m_data->op.packetno = m_data->packet_count++;
        
        ogg_stream_packetin(&m_data->os, &m_data->op);
        flush_ogg_pages(true);

        ogg_stream_clear(&m_data->os);
        speex_bits_destroy(&m_data->bits);
        speex_encoder_destroy(m_data->speex_state);

        if (m_data->file.is_open())
            m_data->file.close();

        m_data->initialized = false;
    }
}