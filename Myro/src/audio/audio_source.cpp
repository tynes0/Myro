#include "audio_source.h"

#include <AL/al.h>
#include <AL/alext.h>
//#include <alc/alcmain.h>
//#include <alhelpers.h>

namespace myro
{
	audio_source::audio_source(uint32_t handle, bool loaded, float length)
		: m_buffer_handle(handle), m_loaded(loaded), m_total_duration(length)
	{

	}

	audio_source::~audio_source()
	{
		// TODO: free openal audio source 
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

	timestamp audio_source::get_length() const
	{
		return timestamp((uint32_t)(m_total_duration / 60.0f), (((uint32_t)m_total_duration) % 60));
	}

	audio_source audio_source::load_from_file(const std::string& filepath, bool spitial)
	{
		return audio_source();
	}
}