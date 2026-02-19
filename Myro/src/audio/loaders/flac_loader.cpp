#include "flac_loader.h"

#include <vector>
#include <memory>
#include <cstdio>

#include "../detail/audio_data.h"
#include "../detail/detail.h"
#include "../detail/openal_backend.h"

#include "ogg_loader.h"

#include <FLAC/stream_decoder.h>

namespace myro
{
    struct flac_loader_data
    {
        std::unique_ptr<FLAC__StreamDecoder, decltype(&FLAC__stream_decoder_delete)> decoder{ nullptr, FLAC__stream_decoder_delete };
    };

    thread_local flac_loader_data s_data;

    struct flac_user_data
    {
        std::vector<int16_t> samples;
        FILE* file = nullptr;
        bool debug_log = false;
        uint32_t sample_rate = 0;
        uint32_t channels = 0;
        bool got_error = false;
    };

    static FLAC__StreamDecoderReadStatus flac_read_cb(const FLAC__StreamDecoder* decoder, FLAC__byte buffer[], size_t* bytes, void* client_data) 
    {
        flac_user_data* userdata = (flac_user_data*)client_data;
        FILE* file = userdata->file;
        if (*bytes > 0) 
        {
            *bytes = fread(buffer, sizeof(FLAC__byte), *bytes, file);
            if (ferror(file))
                return FLAC__STREAM_DECODER_READ_STATUS_ABORT;
            else if (*bytes == 0)
                return FLAC__STREAM_DECODER_READ_STATUS_END_OF_STREAM;
            else
                return FLAC__STREAM_DECODER_READ_STATUS_CONTINUE;
        }
        return FLAC__STREAM_DECODER_READ_STATUS_ABORT;
    }

    static FLAC__StreamDecoderWriteStatus flac_write_cb(const FLAC__StreamDecoder* decoder, const FLAC__Frame* frame, const FLAC__int32* const buffer[], void* client_data)
    {
        flac_user_data* userdata = (flac_user_data*)client_data;

        userdata->sample_rate = frame->header.sample_rate;
        userdata->channels = frame->header.channels;

        const unsigned blocksize = frame->header.blocksize;
        const unsigned channels = frame->header.channels;
        const unsigned bits_per_sample = frame->header.bits_per_sample;

        if (bits_per_sample > 16) 
        {
            if (userdata->debug_log)
                log::warn("FLAC bits_per_sample > 16 ({}), data will be truncated", bits_per_sample);
        }

        for (unsigned i = 0; i < blocksize; ++i) 
        {
            for (unsigned ch = 0; ch < channels; ++ch) 
            {
                FLAC__int32 sample = buffer[ch][i];

                int16_t s16 = static_cast<int16_t>(sample >> (bits_per_sample - 16));
                userdata->samples.push_back(s16);
            }
        }

        return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
    }


    static void flac_error_cb(const FLAC__StreamDecoder* decoder, FLAC__StreamDecoderErrorStatus status, void* client_data) 
    {
        flac_user_data* userdata = (flac_user_data*)client_data;
        if (userdata->debug_log) 
            log::error("FLAC decode error: {}", FLAC__StreamDecoderErrorStatusString[status]);
        userdata->got_error = true;
    }

    static raw_buffer decode_flac(const std::filesystem::path& filepath, bool debug_log, bool is_ogg)
    {
        std::string debug_name = is_ogg ? "Ogg - FLAC" : "FLAC";
        auto stream_init_func = is_ogg ? FLAC__stream_decoder_init_ogg_stream : FLAC__stream_decoder_init_stream;

        std::string fname = filepath.string();

        FILE* file = fopen(fname.c_str(), "rb");
        if (!file)
        {
            if (debug_log)
                log::error("Failed to open {0} file: {1}", debug_name, filepath);
            return nullptr;
        }

        flac_user_data userdata;
        userdata.file = file;
        userdata.debug_log = debug_log;
        userdata.got_error = false;

        FLAC__stream_decoder_set_md5_checking(s_data.decoder.get(), false);

        FLAC__StreamDecoderInitStatus init_status = stream_init_func(
            s_data.decoder.get(),
            flac_read_cb,
            nullptr, // seek callback
            nullptr, // tell callback
            nullptr, // length callback
            nullptr, // eof callback
            flac_write_cb,
            nullptr, // metadata callback
            flac_error_cb,
            &userdata
        );

        if (init_status != FLAC__STREAM_DECODER_INIT_STATUS_OK)
        {
            if (debug_log)
                log::error("Failed to initialize {0} decoder: {}", debug_name, FLAC__StreamDecoderInitStatusString[init_status]);

            fclose(file);
            return nullptr;
        }

        if (!FLAC__stream_decoder_process_until_end_of_stream(s_data.decoder.get()))
        {
            if (debug_log)
                log::error("Failed to process {0} stream", debug_name);

            fclose(file);
            return nullptr;
        }
        if (userdata.got_error)
        {
            fclose(file);
            return nullptr;
        }

        ALenum al_format = openal_backend::get_openAL_format(userdata.channels);
        const float duration_seconds = static_cast<float>(userdata.samples.size()) / (userdata.sample_rate * userdata.channels);

        raw_buffer buffer(userdata.samples.size() * sizeof(int16_t));
        std::memcpy(buffer.data, userdata.samples.data(), buffer.size);

        if (debug_log)
            detail::display_file_info(fname, userdata.channels, userdata.sample_rate, buffer.size);

        audio_data data
        {
            al_format,
            buffer,
            userdata.sample_rate,
            duration_seconds
        };
        raw_buffer buf;
        buf.store(data);

        FLAC__stream_decoder_finish(s_data.decoder.get());
        fclose(file);

        return buf;
    }

    void flac_loader::init(bool debug_log)
    {
        if (!s_data.decoder) 
            s_data.decoder.reset(FLAC__stream_decoder_new());

        if (!s_data.decoder)
        {
            if (debug_log)
                log::error("Failed to create FLAC decoder.");
        }
    }

    void flac_loader::shutdown(bool debug_log)
    {
        MYRO_UNUSED(debug_log);
        s_data.decoder.reset();
    }

    raw_buffer flac_loader::load(const std::filesystem::path& filepath, bool debug_log)
    {
        return decode_flac(filepath, debug_log, ogg_loader::is_ogg_container(filepath));
    }

    raw_buffer flac_loader::load_ogg_flac(const std::filesystem::path& filepath, bool debug_log)
    {
        return decode_flac(filepath, debug_log, true);
    }

}