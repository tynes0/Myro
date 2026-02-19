#include "mp3_loader.h"

#include <minimp3.h>
#include <minimp3_ex.h>

#include "../detail/openal_backend.h"
#include "../detail/audio_data.h"
#include "../detail/detail.h"

namespace myro
{
	struct mp3_loader_data
	{
		mp3dec_t mp3_decoder{};
	};

	thread_local mp3_loader_data s_data;

	void mp3_loader::init(bool debug_log)
	{
		MYRO_UNUSED(debug_log);
		mp3dec_init(&s_data.mp3_decoder);
	}

	void mp3_loader::shutdown(bool debug_log)
	{
		MYRO_UNUSED(debug_log);
		// We don't need to shutdown mp3dec
	}

	raw_buffer mp3_loader::load(const std::filesystem::path& filepath, bool debug_log)
	{
		mp3dec_file_info_t info;
		std::string fname = filepath.string();
		int load_result = mp3dec_load(&s_data.mp3_decoder, fname.c_str(), &info, NULL, NULL);

		if (load_result != 0)
		{
			log::error("Failed to load mp3 file! ({})", filepath);
			return raw_buffer();
		}

		uint32_t size = static_cast<uint32_t>(info.samples * sizeof(mp3d_sample_t));
		int sample_rate = info.hz;
		int channels = info.channels;
		float length_seconds = size / (info.avg_bitrate_kbps * 1024.0f);

		if (debug_log)
			detail::display_file_info(fname, channels, sample_rate, size);

		ALenum al_format = openal_backend::get_openAL_format(channels);

		audio_data data
		{
			al_format,
			raw_buffer(info.buffer, size),
			sample_rate,
			length_seconds
		};

		raw_buffer buf;
		buf.store(data);
		return buf;
	}
}
