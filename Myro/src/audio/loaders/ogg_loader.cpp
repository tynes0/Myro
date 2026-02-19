#include "ogg_loader.h"

#include <fstream>
#include <string_view>
#include <cmath>

#include "../detail/audio_data.h"
#include "../detail/detail.h"
#include "../detail/openal_backend.h"

#include "opus_loader.h"
#include "speex_loader.h"
#include "flac_loader.h"

#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>

namespace myro
{
    struct ogg_loader_data
    {
        raw_buffer audio_scratch_buffer;
    };

    namespace 
    {
        thread_local ogg_loader_data s_data;
        
        bool find_case_insensitive(std::string_view haystack, std::string_view needle) 
        {
            if (needle.size() > haystack.size())
                return false;
    
            for (size_t i = 0; i <= haystack.size() - needle.size(); ++i) 
            {
                bool match = true;
                for (size_t j = 0; j < needle.size(); ++j)
                {
                    if (std::tolower(static_cast<unsigned char>(haystack[i + j])) != std::tolower(static_cast<unsigned char>(needle[j]))) 
                    {
                        match = false;
                        break;
                    }
                }
                if (match) return true;
            }
            return false;
        }
    
        raw_buffer load_vorbis(const std::filesystem::path& filepath)
        {
            std::string fname = filepath.string();
            FILE* file = fopen(fname.c_str(), "rb");
    
            OggVorbis_File vorbis_f;
    
            if (ov_open_callbacks(file, &vorbis_f, nullptr, 0, OV_CALLBACKS_NOCLOSE) < 0)
            {
                log::warn("Couldn't open ogg stream!");
                return raw_buffer{};
            }
    
            vorbis_info* vorbis_i = ov_info(&vorbis_f, -1);
            long sample_rate = vorbis_i->rate;
            int channels = vorbis_i->channels;
    
            uint64_t samples = ov_pcm_total(&vorbis_f, -1);
            float track_length = static_cast<float>(samples) / static_cast<float>(sample_rate);
            uint32_t buffer_size = static_cast<uint32_t>(2 * static_cast<uint64_t>(channels) * samples);
    
            if (s_data.audio_scratch_buffer.size < buffer_size)
                s_data.audio_scratch_buffer.allocate(buffer_size);
    
            raw_buffer ogg_buffer(s_data.audio_scratch_buffer, shallow_copy{});
            raw_buffer buffer_ptr(ogg_buffer, shallow_copy{});
    
            int eof = 0;
    
            while (!eof)
            {
                int current_section;
                long length = ov_read(&vorbis_f, buffer_ptr.as<char>(), 4096, 0, 2, 1, &current_section);
                buffer_ptr.data += length;
                if (length == 0)
                {
                    eof = 1;
                }
                else if (length < 0)
                {
                    MYRO_ASSERT(length != OV_EBADLINK, "Corrupt bitsream section!");
                    ov_clear(&vorbis_f);
                    detail::fclose_checked(file);
                    return raw_buffer{};
                }
            }
    
            uint32_t size = static_cast<uint32_t>(buffer_ptr - ogg_buffer);
            ogg_buffer.size = size;
            MYRO_ASSERT(buffer_size == size, "The bitstream was not read to the expected size.");
    
            ALenum al_format = openal_backend::get_openAL_format(channels);
    
            audio_data data
            {
                .al_format = al_format,
                .buffer = ogg_buffer,
                .sample_rate = sample_rate,
                .track_length = track_length
            };
    
            raw_buffer buf;
            buf.store(data);
            return buf;
        }
    }
    
    void ogg_loader::init()
    {
        s_data.audio_scratch_buffer.allocate(constants::initial_scratch_buffer_size);
    }

    void ogg_loader::shutdown()
    {
        s_data.audio_scratch_buffer.release();
    }

    raw_buffer myro::ogg_loader::load(const std::filesystem::path& path)
    {
        switch (ogg_codec_type codec_type = detect_ogg_codec_robust(path); codec_type)
        {
        case ogg_codec_type::vorbis: return load_vorbis(path);
        case ogg_codec_type::opus:   return opus_loader::load_ogg_opus(path);
        case ogg_codec_type::speex:  return speex_loader::load_ogg_speex(path);
        case ogg_codec_type::flac:   return flac_loader::load_ogg_flac(path);
        case ogg_codec_type::unknown:
            break;
        }
        log::error("Unknown codec type! File: {}", path);
        return raw_buffer{};
    }

    ogg_codec_type ogg_loader::detect_ogg_codec_robust(const std::filesystem::path& path)
    {
        std::ifstream file(path, std::ios::binary);
        if (!file)
            return ogg_codec_type::unknown;

        char page_header[27] = {};
        file.read(page_header, sizeof(page_header));
        if (file.gcount() < 27)
            return ogg_codec_type::unknown;

        if (std::string_view(page_header, 4) != "OggS")
            return ogg_codec_type::unknown;

        uint8_t segment_count = static_cast<uint8_t>(page_header[26]);
        std::vector<uint8_t> lacing_values(segment_count);
        file.read(reinterpret_cast<char*>(lacing_values.data()), segment_count);

        if (file.gcount() < segment_count)
            return ogg_codec_type::unknown;

        size_t payload_size = 0;
        for (auto len : lacing_values)
            payload_size += len;

        std::vector<char> payload(payload_size);
        file.read(payload.data(), static_cast<std::streamsize>(payload_size));
        if (file.gcount() < static_cast<std::streamsize>(payload_size))
            return ogg_codec_type::unknown;

        std::string_view data(payload.data(), payload.size());

        if (find_case_insensitive(data, "OpusHead"))
            return ogg_codec_type::opus;
        if (find_case_insensitive(data, "vorbis"))
            return ogg_codec_type::vorbis;
        if (find_case_insensitive(data, "Speex"))
            return ogg_codec_type::speex;
        if (find_case_insensitive(data, "FLAC"))
            return ogg_codec_type::flac;

        return ogg_codec_type::unknown;
    }

    bool ogg_loader::is_ogg_container(const std::filesystem::path& path)
    {
        std::ifstream file(path, std::ios::binary);
        if (!file) return false;

        char magic[4] = {};
        file.read(magic, 4);
        return file.gcount() == 4 && std::memcmp(magic, "OggS", 4) == 0;
    }
}