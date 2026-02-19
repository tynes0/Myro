#pragma once

#include "../math/vec3.h"
#include "../core/timestamp.h"

#include <cstdint>
#include <string>
#include <memory>

namespace myro 
{
	class audio_source
	{
	public:
		audio_source() = default;
		audio_source(const audio_source&) = default;
		audio_source(audio_source&&) = default;
		audio_source& operator=(const audio_source&) = default;
		~audio_source();

		void unload();

		void set_position(const vec3& pos);
		void set_gain(float gain);
		void set_pitch(float pitch);
		void set_spitial(bool spitial);
		void set_loop(bool loop);

		void increase_gain(float increment_v);
		void decrease_gain(float decrement_v);

		void increase_pitch(float increment_v);
		void decrease_pitch(float decrement_v);

		float get_current_duration() const;
		vec3 get_position() const { return m_position; }
		float get_gain() const { return m_gain; }
		float get_pitch() const { return m_pitch; }
		float get_length() const { return m_total_duration; }
		bool is_spitial() const { return m_spitial; }
		bool is_loop() const { return m_loop; }

		timestamp get_timestamp() const;

		bool is_loaded() const { return m_loaded; }

		static std::shared_ptr<audio_source> load_from_file(const std::filesystem::path& filepath, bool spitial = false);
	private:
		audio_source(uint32_t handle, bool loaded, float length);

		uint32_t m_buffer_handle = 0;
		uint32_t m_source_handle = 0;

		bool m_loaded = false;

		float m_total_duration = 0.0f; // in seconds

		// attributes
		vec3 m_position;
		float m_gain = 0.0f;
		float m_pitch = 0.0f;
		bool m_loop = false;
		bool m_spitial = false;

		friend class audio_engine;
		friend class audio_effect_manager;
		friend class audio_filter_manager;
	};
}