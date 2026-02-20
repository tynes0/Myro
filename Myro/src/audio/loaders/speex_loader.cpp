#include "audio/loaders/speex_loader.h"
#include "audio/loaders/ogg_loader.h"

#include "internal/audio_data.h"
#include "internal/openal_backend.h"

#include <speex/speex.h>
#include <speex/speex_header.h>
#include <speex/speex_stereo.h>
#include <ogg/ogg.h>

#include <fstream>
#include <vector>

namespace myro
{
    void speex_loader::init()
    {
    }

    void speex_loader::shutdown()
    {
    }

    raw_buffer speex_loader::load(const std::filesystem::path& filepath)
    {
        if (ogg_loader::is_ogg_container(filepath))
            return load_ogg_speex(filepath);
        
        log::error("Unsupported Speex format: {}", filepath.string());
        return raw_buffer{};
    }

    raw_buffer speex_loader::load_ogg_speex(const std::filesystem::path& filepath)
    {
        std::ifstream file(filepath, std::ios::binary);
        if (!file.is_open())
        {
            log::error("Speex file could not open: {}", filepath.string());
            return raw_buffer{};
        }

        ogg_sync_state oy;
        ogg_stream_state os;
        ogg_page og;
        ogg_packet op;
        
        void* st = nullptr;
        SpeexBits bits;
        
        speex_bits_init(&bits);
        ogg_sync_init(&oy);

        int channels = 1;
        int sample_rate = 8000;
        int frame_size = 0;
        int packet_count = 0;
        bool stream_init = false;
        
        std::vector<int16_t> pcm_data;

        char* buffer;
        int bytes_read;

        while (true)
        {
            static constexpr int CHUNK_SIZE = 4096;
            buffer = ogg_sync_buffer(&oy, CHUNK_SIZE);
            file.read(buffer, CHUNK_SIZE);
            bytes_read = static_cast<int>(file.gcount());
            ogg_sync_wrote(&oy, bytes_read);

            if (bytes_read == 0 && ogg_sync_check(&oy) == 0) // EOF
                break;

            while (ogg_sync_pageout(&oy, &og) == 1)
            {
                if (!stream_init)
                {
                    ogg_stream_init(&os, ogg_page_serialno(&og));
                    stream_init = true;
                }

                ogg_stream_pagein(&os, &og);

                while (ogg_stream_packetout(&os, &op) == 1)
                {
                    packet_count++;

                    if (packet_count == 1)
                    {
                        SpeexHeader* header = speex_packet_to_header(reinterpret_cast<char*>(op.packet), op.bytes);
                        if (!header)
                        {
                            log::error("Speex header could not read or invalid: {}", filepath.string());
                            goto cleanup;
                        }

                        const SpeexMode* mode = speex_lib_get_mode(header->mode);
                        st = speex_decoder_init(mode);

                        int enh = 1; // Perceptual enhancement
                        speex_decoder_ctl(st, SPEEX_SET_ENH, &enh);
                        speex_decoder_ctl(st, SPEEX_GET_FRAME_SIZE, &frame_size);
                        
                        speex_decoder_ctl(st, SPEEX_SET_SAMPLING_RATE, &header->rate);
                        
                        sample_rate = header->rate;
                        channels = header->nb_channels;

                        speex_header_free(header);
                    }
                    else if (packet_count == 2)
                    {
                        // 2. Packet: Comment/Metadata
                    }
                    else
                    {
                        speex_bits_read_from(&bits, reinterpret_cast<const char*>(op.packet), op.bytes);

                        std::vector<int16_t> frame_output(static_cast<size_t>(frame_size) * channels);
                        
                        if (channels == 2)
                        {
                            SpeexStereoState stereo = SPEEX_STEREO_STATE_INIT;
                            speex_decode_int(st, &bits, frame_output.data());
                            speex_decode_stereo_int(frame_output.data(), frame_size, &stereo);
                        } else
                        {
                            speex_decode_int(st, &bits, frame_output.data());
                        }

                        pcm_data.insert(pcm_data.end(), frame_output.begin(), frame_output.end());
                    }
                }
            }
        }

cleanup:
        if (st) speex_decoder_destroy(st);
        speex_bits_destroy(&bits);
        if (stream_init) ogg_stream_clear(&os);
        ogg_sync_clear(&oy);
        file.close();

        if (pcm_data.empty())
        {
            log::error("PCM data could not be read from the Speex file: {}", filepath.string());
            return raw_buffer{};
        }

        uint64_t total_frames = pcm_data.size() / channels;
        ALenum al_format = openal_backend::get_openAL_format(channels);
        
        size_t byte_size = pcm_data.size() * sizeof(int16_t);
        raw_buffer pcm_buf(byte_size);
        std::memcpy(pcm_buf.as<void>(), pcm_data.data(), byte_size);

        audio_data data {
            .al_format = al_format,
            .buffer = pcm_buf,
            .sample_rate = sample_rate,
            .track_length = (static_cast<float>(total_frames) / static_cast<float>(sample_rate))
        };

        raw_buffer final_buf;
        final_buf.store(data);
        return final_buf;
    }
}
