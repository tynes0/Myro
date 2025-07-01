#pragma once

#include "../math/vec3.h"
#include "../core/timestamp.h"

#include <cstdint>
#include <string>

namespace myro
{
	class audio_source
	{
	public:
		~audio_source();

		void set_position(const vec3& pos);
		void set_gain(float gain);
		void set_pitch(float pitch);
		void set_spitial(bool spitial);
		void set_loop(bool loop);

		timestamp get_length() const;

		bool is_loaded() const { return m_loaded; }

		static audio_source load_from_file(const std::string& filepath, bool spitial = false);
	private:
		audio_source() = default;
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
	};
}