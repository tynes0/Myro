#include "audio/audio_source.h"
#include "audio/audio_engine.h"

#include <AL/al.h>
#include <AL/alext.h>

namespace myro
{
	audio_source::audio_source(uint32_t handle, bool loaded, float length)
		: m_buffer_handle(handle), m_loaded(loaded), m_total_duration(length)
	{
	}

	audio_source::~audio_source()
	{
		unload();
	}

	void audio_source::unload()
	{
		if (m_loaded && audio_engine::is_active())
		{
			alSourceStop(m_source_handle);
			alSourcei(m_source_handle, AL_BUFFER, 0);

			ALuint buffer = m_buffer_handle;
			if (buffer != 0)
				alDeleteBuffers(1, &buffer);

			alDeleteSources(1, &m_source_handle);

			m_source_handle = 0;
			m_buffer_handle = 0;
			m_loaded = false;
			m_total_duration = 0.0f;

			if (alGetError() != AL_NO_ERROR)
				log::error("Failed to unload audio source.");
		}
	}

	void audio_source::set_position(const vec3& pos)
	{
		m_position = pos;

		alSourcefv(m_source_handle, AL_POSITION, pos.v);
	}

	void audio_source::set_gain(float gain)
	{
		m_gain = gain;

		alSourcef(m_source_handle, AL_GAIN, gain);
	}

	void audio_source::set_pitch(float pitch)
	{
		m_pitch = pitch;

		alSourcef(m_source_handle, AL_PITCH, pitch);
	}

	void audio_source::set_spitial(bool spitial)
	{
		m_spitial = spitial;

		alSourcei(m_source_handle, AL_SOURCE_SPATIALIZE_SOFT, spitial ? AL_TRUE : AL_FALSE);
		alDistanceModel(AL_INVERSE_DISTANCE_CLAMPED);
	}

	void audio_source::set_loop(bool loop)
	{
		m_loop = loop;

		alSourcei(m_source_handle, AL_LOOPING, loop ? AL_TRUE : AL_FALSE);
	}

	void audio_source::increase_gain(float increment_v)
	{
		m_gain += increment_v;

		alSourcef(m_source_handle, AL_GAIN, m_gain);
	}

	void audio_source::decrease_gain(float decrement_v)
	{
		if (m_gain - decrement_v < 0.0f)
			m_gain = 0.0f;
		else
			m_gain -= decrement_v;

		alSourcef(m_source_handle, AL_GAIN, m_gain);
	}

	void audio_source::increase_pitch(float increment_v)
	{
		if (m_pitch + increment_v > 2.0f)
			m_pitch = 2.0f;
		else
			m_pitch += increment_v;

		alSourcef(m_source_handle, AL_PITCH, m_pitch);
	}

	void audio_source::decrease_pitch(float decrement_v)
	{
		if (m_pitch - decrement_v < 0.5f)
			m_pitch = 0.5f;
		else
			m_pitch -= decrement_v;

		alSourcef(m_source_handle, AL_PITCH, m_pitch);
	}

	float audio_source::get_current_duration() const
	{
		ALfloat duration;
		alGetSourcef(m_source_handle, AL_SEC_OFFSET, &duration);
		return static_cast<float>(duration);
	}

	timestamp audio_source::get_timestamp() const
	{
		return timestamp{
			.minutes = static_cast<uint32_t>(m_total_duration / 60.0f),
			.seconds = (static_cast<uint32_t>(m_total_duration) % 60)
		};
	}

	std::shared_ptr<audio_source> audio_source::load_from_file(const std::filesystem::path& filepath, bool spitial)
	{
		std::shared_ptr<audio_source> result = audio_engine::load_audio_source(filepath);

		if(result)
			result->set_spitial(spitial);

		return result;
	}
}