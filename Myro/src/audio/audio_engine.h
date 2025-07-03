#pragma once

#include <filesystem>
#include <memory>

#include "audio_source.h"
#include "audio_state.h"
#include "../core/buffer.h"

namespace myro
{
	class audio_engine
	{
	public:
		static void init();
		static void shutdown();

		static std::shared_ptr<audio_source> load_audio_source(const std::filesystem::path& filepath);
		static void unload_audio_source(std::shared_ptr<audio_source> source);

		static void play(const std::shared_ptr<audio_source>& source);
		static void stop(const std::shared_ptr<audio_source>& source);
		static void pause(const std::shared_ptr<audio_source>& source);
		static void rewind(const std::shared_ptr<audio_source>& source);
		static void seek(const std::shared_ptr<audio_source>& source, float seconds);

		static void set_doppler_factor(float factor); // 1.0f by default
		static void set_speed_of_sound(float speed); // 343.3f by default

		static audio_state state_of(const std::shared_ptr<audio_source>& source);

		static void set_debug_logging(bool log); // false by default
		static bool is_debug_logging(); // false by default
	private:
		static std::shared_ptr<audio_source> load_audio_source_al(raw_buffer buf);
	};
}