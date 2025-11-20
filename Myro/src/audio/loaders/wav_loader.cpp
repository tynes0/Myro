#include "wav_loader.h"

#include "../detail/openal_backend.h"
#include "../detail/audio_data.h"
#include "../detail/detail.h"

#include <miniaudio.h>

namespace myro
{
	void wav_loader::init(bool debug_log)
	{
		MYRO_UNUSED(debug_log);
		// miniaudio does not need a basic initialization 
		// as it requires a separate initialization for each file.
	}

	void wav_loader::shutdown(bool debug_log)
	{
		MYRO_UNUSED(debug_log);
		// For the same reasons as the init method,
		// there is no need to shutdown either.
	}

	raw_buffer wav_loader::load(const std::filesystem::path& filepath, bool debug_log)
	{
		std::string fname = filepath.string();

		ma_result result;
		ma_decoder_config config = ma_decoder_config_init_default();
		ma_decoder decoder;

		result = ma_decoder_init_file(fname.c_str(), &config, &decoder);
		if (result != MA_SUCCESS)
		{
			log::error("Failed to load WAV file: {}", fname);
			return raw_buffer();
		}

		ma_uint64 total_frames = 0;
		result = ma_decoder_get_length_in_pcm_frames(&decoder, &total_frames);
		if (result != MA_SUCCESS)
		{
			log::error("Failed to get the length of the WAV file.");
			ma_decoder_uninit(&decoder);
			return raw_buffer();
		}

		size_t buffer_size = decoder.outputChannels * total_frames * sizeof(ma_int16);

		if (debug_log)
			detail::display_file_info(fname, decoder.outputChannels, decoder.outputSampleRate, buffer_size);

		raw_buffer buffer(buffer_size);
		size_t frames_read = 0;
		result = ma_decoder_read_pcm_frames(&decoder, buffer.as<void>(), total_frames, &frames_read);
		if (result != MA_SUCCESS)
		{
			log::error("Failed to read WAV frames.");
			ma_decoder_uninit(&decoder);
			buffer.release();
			return raw_buffer();
		}

		if (frames_read != total_frames)
			log::warn("Not all frames were read. Expected {}, got {}", total_frames, frames_read);

		ALenum al_format = openal_backend::get_openAL_format(decoder.outputChannels);

		audio_data data
		{
			al_format,
			buffer,
			static_cast<int>(decoder.outputSampleRate),
			(static_cast<float>(total_frames) / static_cast<float>(decoder.outputSampleRate))
		};

		raw_buffer buf;
		buf.store(data);
		return buf;
	}
}

