#include "audio/loaders/flac_loader.h"

#include "audio/loaders/ogg_loader.h"
#include "internal/audio_data.h"
#include "internal/detail.h"
#include "internal/openal_backend.h"

#include <FLAC/stream_decoder.h>

#include <vector>
#include <memory>
#include <cstdio>

namespace myro
{
    struct flac_loader_data
    {
        std::unique_ptr<FLAC__StreamDecoder, decltype(&FLAC__stream_decoder_delete)> decoder{ nullptr, FLAC__stream_decoder_delete };
    };

    namespace
    {
        thread_local flac_loader_data s_data;
    
        struct flac_user_data
        {
            std::vector<int16_t> samples;
            FILE* file = nullptr;
            uint32_t sample_rate = 0;
            uint32_t channels = 0;
            bool got_error = false;
        };
    
        FLAC__StreamDecoderReadStatus flac_read_cb(const FLAC__StreamDecoder* decoder, FLAC__byte buffer[], size_t* bytes, void* client_data) 
        {
            MYRO_UNUSED(decoder);
            flac_user_data* userdata = static_cast<flac_user_data*>(client_data);
            FILE* file = userdata->file;
            if (*bytes > 0) 
            {
                *bytes = fread(buffer, sizeof(FLAC__byte), *bytes, file);
                if (ferror(file))
                    return FLAC__STREAM_DECODER_READ_STATUS_ABORT;
                if (*bytes == 0)
                    return FLAC__STREAM_DECODER_READ_STATUS_END_OF_STREAM;
                return FLAC__STREAM_DECODER_READ_STATUS_CONTINUE;
            }
            return FLAC__STREAM_DECODER_READ_STATUS_ABORT;
        }

        FLAC__StreamDecoderWriteStatus flac_write_cb(const FLAC__StreamDecoder* decoder, const FLAC__Frame* frame, const FLAC__int32* const buffer[], void* client_data)
        {
            MYRO_UNUSED(decoder);
            flac_user_data* userdata = static_cast<flac_user_data*>(client_data);
            
            userdata->sample_rate = frame->header.sample_rate;
            userdata->channels = frame->header.channels;

            const unsigned blocksize = frame->header.blocksize;
            const unsigned channels = frame->header.channels;
            const unsigned bits_per_sample = frame->header.bits_per_sample;

            if (bits_per_sample > 16) 
                log::warn("FLAC bits_per_sample > 16 ({}), data will be truncated", bits_per_sample);

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


        void flac_error_cb(const FLAC__StreamDecoder* decoder, FLAC__StreamDecoderErrorStatus status, void* client_data) 
        {
            MYRO_UNUSED(decoder);
            flac_user_data* userdata = static_cast<flac_user_data*>(client_data);
            log::error("FLAC decode error: {}", FLAC__StreamDecoderErrorStatusString[status]);
            userdata->got_error = true;
        }

        raw_buffer decode_flac(const std::filesystem::path& filepath, bool is_ogg)
        {
            std::string debug_name = is_ogg ? "Ogg - FLAC" : "FLAC";
            auto stream_init_func = is_ogg ? FLAC__stream_decoder_init_ogg_stream : FLAC__stream_decoder_init_stream;

            std::string fname = filepath.string();

            FILE* file = fopen(fname.c_str(), "rb");
            if (!file)
            {
                log::error("Failed to open {0} file: {1}", debug_name, filepath);
                return nullptr;
            }

            flac_user_data userdata;
            userdata.file = file;
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
                log::error("Failed to initialize {0} decoder: {}", debug_name, FLAC__StreamDecoderInitStatusString[init_status]);
                detail::fclose_checked(file);
                return nullptr;
            }

            if (!FLAC__stream_decoder_process_until_end_of_stream(s_data.decoder.get()))
            {
                log::error("Failed to process {0} stream", debug_name);
                detail::fclose_checked(file);
                return nullptr;
            }
            if (userdata.got_error)
            {
                detail::fclose_checked(file);
                return nullptr;
            }

            ALenum al_format = openal_backend::get_openAL_format(userdata.channels);
            const float duration_seconds = static_cast<float>(userdata.samples.size()) / static_cast<float>(userdata.sample_rate * userdata.channels);

            raw_buffer buffer(userdata.samples.size() * sizeof(int16_t));
            std::memcpy(buffer.data, userdata.samples.data(), buffer.size);

            audio_data data
            {
                .al_format = al_format,
                .buffer = buffer,
                .sample_rate = userdata.sample_rate,
                .track_length = duration_seconds
            };
            raw_buffer buf;
            buf.store(data);

            FLAC__stream_decoder_finish(s_data.decoder.get());
            detail::fclose_checked(file);

            return buf;
        }
    }

    void flac_loader::init()
    {
        if (!s_data.decoder) 
            s_data.decoder.reset(FLAC__stream_decoder_new());

        if (!s_data.decoder)
            log::error("Failed to create FLAC decoder.");
    }

    void flac_loader::shutdown()
    {
        s_data.decoder.reset();
    }

    raw_buffer flac_loader::load(const std::filesystem::path& filepath)
    {
        return decode_flac(filepath, ogg_loader::is_ogg_container(filepath));
    }

    raw_buffer flac_loader::load_ogg_flac(const std::filesystem::path& filepath)
    {
        return decode_flac(filepath, true);
    }
}