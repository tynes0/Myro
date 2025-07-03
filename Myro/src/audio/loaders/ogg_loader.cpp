#include "ogg_loader.h"

#include <fstream>
#include <string_view>

#include "../detail/audio_data.h"
#include "../detail/detail.h"
#include "../detail/openal_backend.h"

#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>

#define INITIAL_SCRATCH_BUFFER_SIZE 10 * 1024 * 1024 // 10 mb

namespace myro
{
    struct ogg_loader_data
    {
        raw_buffer audio_scratch_buffer;
    };

    static ogg_loader_data s_data;

    static bool find_case_insensitive(std::string_view haystack, std::string_view needle) {
        if (needle.size() > haystack.size()) return false;

        for (size_t i = 0; i <= haystack.size() - needle.size(); ++i) {
            bool match = true;
            for (size_t j = 0; j < needle.size(); ++j) {
                if (std::tolower(static_cast<unsigned char>(haystack[i + j])) !=
                    std::tolower(static_cast<unsigned char>(needle[j]))) {
                    match = false;
                    break;
                }
            }
            if (match) return true;
        }
        return false;
    }

    static raw_buffer load_vorbis(const std::filesystem::path& filepath, bool debug_log)
    {
        std::string fname = filepath.string();
        FILE* file = fopen(fname.c_str(), "rb");

        OggVorbis_File vorbis_f;

        if (ov_open_callbacks(file, &vorbis_f, NULL, 0, OV_CALLBACKS_NOCLOSE) < 0)
        {
            log::warn("Couldn't open ogg stream!");
            return raw_buffer();
        }

        vorbis_info* vorbis_i = ov_info(&vorbis_f, -1);
        long sample_rate = vorbis_i->rate;
        int channels = vorbis_i->channels;

        uint64_t samples = ov_pcm_total(&vorbis_f, -1);
        float track_length = static_cast<float>(samples) / static_cast<float>(sample_rate);
        uint32_t buffer_size = static_cast<uint32_t>(2 * static_cast<uint64_t>(channels) * samples);

        if (debug_log)
            detail::display_file_info(fname, channels, sample_rate, buffer_size);

        if (s_data.audio_scratch_buffer.size < buffer_size)
            s_data.audio_scratch_buffer.allocate(buffer_size);

        raw_buffer ogg_buffer = s_data.audio_scratch_buffer;
        raw_buffer buffer_ptr = ogg_buffer;

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
                fclose(file);
                return raw_buffer();
            }
        }

        uint32_t size = static_cast<uint32_t>(buffer_ptr - ogg_buffer);
        ogg_buffer.size = size;
        MYRO_ASSERT(buffer_size == size, "The bitstream was not read to the expected size.");

        ALenum al_format = openal_backend::get_openAL_format(channels);

        audio_data data
        {
            al_format,
            ogg_buffer,
            sample_rate,
            track_length
        };

        raw_buffer buf;
        buf.store(data);
        return buf;
    }

    static raw_buffer load_opus(const std::filesystem::path& filepath, bool debug_log)
    {
        return raw_buffer();
    }

    static raw_buffer load_speex(const std::filesystem::path& filepath, bool debug_log)
    {
        return raw_buffer();
    }

    static raw_buffer load_flac(const std::filesystem::path& filepath, bool debug_log)
    {
        return raw_buffer();
    }

    void ogg_loader::init()
    {
        s_data.audio_scratch_buffer.allocate(INITIAL_SCRATCH_BUFFER_SIZE);
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
        file.read(payload.data(), payload_size);
        if (file.gcount() < static_cast<std::streamsize>(payload_size))
            return ogg_codec_type::unknown;

        std::string_view data(payload.data(), payload.size());

        if (data.find("OpusHead") != std::string_view::npos)
            return ogg_codec_type::opus;
        if (data.find("vorbis") != std::string_view::npos)
            return ogg_codec_type::vorbis;
        if (find_case_insensitive(data, "Speex"))
            return ogg_codec_type::speex;
        if (find_case_insensitive(data, "FLAC")) 
            return ogg_codec_type::flac;

        return ogg_codec_type::unknown;
    }

    raw_buffer myro::ogg_loader::load(const std::filesystem::path& path, bool debug_log)
    {
        ogg_codec_type codec_type = detect_ogg_codec_robust(path);

        switch (codec_type)
        {
        case myro::ogg_codec_type::vorbis: return load_vorbis(path, debug_log);
        case myro::ogg_codec_type::opus:   return load_opus(path, debug_log);
        case myro::ogg_codec_type::speex:  return load_speex(path, debug_log);
        case myro::ogg_codec_type::flac:   return load_flac(path, debug_log);
        case myro::ogg_codec_type::unknown:
        default:
            log::error("Unknown codec type! File: {}", path);
            return raw_buffer();
        }

        audio_data data;
    }
}