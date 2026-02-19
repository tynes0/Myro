#pragma once

#include <filesystem>
#include <vector>
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

		static bool is_active();

		static void set_thread_count(uint32_t count);
		static uint32_t get_thread_count();
		static uint32_t get_max_thread_count();

		static std::shared_ptr<audio_source> load_audio_source(const std::filesystem::path& filepath);
		static std::vector<std::shared_ptr<audio_source>> multi_load_audio_source(const std::vector<std::filesystem::path>& filepaths);

		static void unload_audio_source(const std::shared_ptr<audio_source>& source);
		static void multi_unload_audio_source(const std::vector<std::shared_ptr<audio_source>>& sources);

		static void cleanup_expired_sources();

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