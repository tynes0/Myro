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

	namespace { thread_local mp3_loader_data s_data; }

	void mp3_loader::init()
	{
		mp3dec_init(&s_data.mp3_decoder);
	}

	void mp3_loader::shutdown()
	{
		// We don't need to shut down mp3dec
	}

	raw_buffer mp3_loader::load(const std::filesystem::path& filepath)
	{
		mp3dec_file_info_t info;
		std::string fname = filepath.string();
		int load_result = mp3dec_load(&s_data.mp3_decoder, fname.c_str(), &info, nullptr, nullptr);

		if (load_result != 0)
		{
			log::error("Failed to load mp3 file! ({})", filepath);
			return raw_buffer{};
		}

		uint32_t size = static_cast<uint32_t>(info.samples * sizeof(mp3d_sample_t));
		int sample_rate = info.hz;
		int channels = info.channels;
		float length_seconds = static_cast<float>(size) / (static_cast<float>(info.avg_bitrate_kbps) * 1024.0f);

		ALenum al_format = openal_backend::get_openAL_format(channels);

		audio_data data
		{
			.al_format = al_format,
			.buffer = raw_buffer(info.buffer, size),
			.sample_rate = sample_rate,
			.track_length = length_seconds
		};

		raw_buffer buf;
		buf.store(data);
		return buf;
	}
}
