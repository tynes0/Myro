#include "audio/loaders/opus_loader.h"

#include "internal/audio_data.h"
#include "internal/detail.h"
#include "internal/openal_backend.h"

#include <opusfile.h>

namespace myro
{
    struct opus_loader_data
    {
        raw_buffer audio_scratch_buffer;
    };

    namespace { thread_local opus_loader_data s_data; }

    void opus_loader::init()
    {
        s_data.audio_scratch_buffer.allocate(constants::initial_scratch_buffer_size);
    }

    void opus_loader::shutdown()
    {
        s_data.audio_scratch_buffer.release();
    }

    raw_buffer opus_loader::load(const std::filesystem::path& filepath)
    {
        // Most .opus files are actually ogg containers
        return load_ogg_opus(filepath);
    }

    raw_buffer opus_loader::load_ogg_opus(const std::filesystem::path& filepath)
    {
        std::string fname = filepath.string();

        int test_error = 0;
        OggOpusFile* test_file = op_test_file(fname.c_str(), &test_error);
        if (!test_file)
        {
            log::error("File is not a valid Ogg Opus stream: {} (error code: {})", fname, test_error);
            return raw_buffer{};
        }
        op_free(test_file);

        int error = 0;
        OggOpusFile* opus_f = op_open_file(fname.c_str(), &error);
        if (!opus_f)
        {
            log::error("Couldn't open Opus stream after test! Error: {}", error);
            return raw_buffer{};
        }

        const OpusHead* head = op_head(opus_f, -1);
        if (!head)
        {
            log::error("Couldn't read Opus header for file: {}", fname);
            op_free(opus_f);
            return raw_buffer{};
        }

        opus_uint32 sample_rate = head->input_sample_rate;
        int channels = head->channel_count;

        int64_t total_pcm_samples = op_pcm_total(opus_f, -1);
        float track_length = static_cast<float>(total_pcm_samples) / static_cast<float>(sample_rate);

        uint32_t buffer_size = static_cast<uint32_t>(2 * static_cast<uint64_t>(channels) * total_pcm_samples);

        if (s_data.audio_scratch_buffer.size < buffer_size)
            s_data.audio_scratch_buffer.allocate(buffer_size);

        raw_buffer& pcm_buf = s_data.audio_scratch_buffer;
        int16_t* pcm = pcm_buf.as<int16_t>();
        size_t pcm_index = 0;

        float pcm_float_buffer[constants::pcm_flt_buffer_size * 2]; // support up to 2 channels

        while (true)
        {
            int result = op_read_float(opus_f, pcm_float_buffer, static_cast<int>(constants::pcm_flt_buffer_size) * channels, nullptr);
            if (result < 0)
            {
                log::error("Error reading Opus stream: {}", result);
                op_free(opus_f);
                return raw_buffer{};
            }

            if (result == 0)
                break;

            size_t sample_count = static_cast<size_t>(result) * static_cast<size_t>(channels);
            if ((pcm_index + sample_count) * sizeof(int16_t) > pcm_buf.size)
            {
                log::error("Opus buffer overflow during conversion!");
                op_free(opus_f);
                return raw_buffer{};
            }

            for (size_t i = 0; i < sample_count; ++i)
                pcm[pcm_index++] = static_cast<int16_t>(std::clamp(pcm_float_buffer[i] * 32767.0f, -32768.0f, 32767.0f));
        }

        op_free(opus_f);

        raw_buffer pcm_final;
        pcm_final.data = reinterpret_cast<uint8_t*>(pcm);
        pcm_final.size = (pcm_index * sizeof(int16_t));

        ALenum al_format = openal_backend::get_openAL_format(channels);

        audio_data data
        {
            .al_format = al_format,
            .buffer = pcm_final,
            .sample_rate = sample_rate,
            .track_length = track_length
        };

        raw_buffer result;
        result.store(data);
        return result;
    }
}
