#include "audio_engine.h"

#include <cstdio>

#include "detail/audio_data.h"
#include "detail/openal_backend.h"

#include "listener.h"
#include "audio_file_format.h"

#include "loaders/ogg_loader.h"
#include "loaders/mp3_loader.h"
#include "loaders/wav_loader.h"

#include "../core/buffer.h"
#include "../core/log.h"

#include <AL/al.h>
#include <AL/alext.h>
#include <alc/alcmain.h>

namespace myro
{
	struct engine_data
	{
		bool debug_log = false;
	};

	static engine_data s_data;

	void audio_engine::init()
	{
		openal_backend::init();

		mp3_loader::init();
		ogg_loader::init();
		wav_loader::init();

		listener::init();
	}

	void audio_engine::shutdown()
	{
		// TODO: Looks like this function needs an update.

		openal_backend::shutdown();
	}

	std::shared_ptr<audio_source> audio_engine::load_audio_source(const std::filesystem::path& filepath)
	{
		auto format = get_file_format(filepath);

		raw_buffer buf;

		switch (format)
		{
		case audio_file_format::ogg: buf = ogg_loader::load(filepath, s_data.debug_log); break;
		case audio_file_format::mp3: buf = mp3_loader::load(filepath, s_data.debug_log); break;
		case audio_file_format::wav: buf = wav_loader::load(filepath, s_data.debug_log); break;
		}

		if (!buf)
		{
			log::error("Error while loading audio source!");
			return nullptr;
		}

		return load_audio_source_al(buf);
	}

	void audio_engine::unload_audio_source(std::shared_ptr<audio_source> source)
	{
		if (!source)
		{
			log::error("Unloaded audio source passed to audio engine!");
			return;
		}

		if (source->m_loaded)
		{
			alSourceStop(source->m_source_handle);
			alSourcei(source->m_source_handle, AL_BUFFER, 0);

			ALuint buffer = source->m_buffer_handle;
			if (buffer != 0)
				alDeleteBuffers(1, &buffer);

			alDeleteSources(1, &source->m_source_handle);

			source->m_source_handle = 0;
			source->m_buffer_handle = 0;
			source->m_loaded = false;
			source->m_total_duration = 0.0f;

			if (alGetError() != AL_NO_ERROR)
				log::error("Failed to unload audio source.");
		}
	}

	void audio_engine::play(const std::shared_ptr<audio_source>& source)
	{
		if (!source || !source->m_loaded)
		{
			log::error("Unloaded audio source passed to audio engine!");
			return;
		}

		alSourcePlay(source->m_source_handle);
	}

	void audio_engine::stop(const std::shared_ptr<audio_source>& source)
	{
		if (!source || !source->m_loaded)
		{
			log::error("Unloaded audio source passed to audio engine!");
			return;
		}

		alSourceStop(source->m_source_handle);
	}

	void audio_engine::pause(const std::shared_ptr<audio_source>& source)
	{
		if (!source || !source->m_loaded)
		{
			log::error("Unloaded audio source passed to audio engine!");
			return;
		}

		alSourcePause(source->m_source_handle);
	}

	void audio_engine::rewind(const std::shared_ptr<audio_source>& source)
	{
		if (!source || !source->m_loaded)
		{
			log::error("Unloaded audio source passed to audio engine!");
			return;
		}

		alSourceRewind(source->m_source_handle);
	}

	void audio_engine::seek(const std::shared_ptr<audio_source>& source, float seconds)
	{
		if (!source || !source->m_loaded)
		{
			log::error("Unloaded audio source passed to audio engine!");
			return;
		}

		if (seconds < 0 || seconds > source->m_total_duration)
		{
			log::warn("Seek position out of range: {}", seconds);
			return;
		}

		alSourcef(source->m_source_handle, AL_SEC_OFFSET, seconds);
	}

	void audio_engine::set_doppler_factor(float factor)
	{
		alDopplerFactor(factor);

		if (alGetError() != AL_NO_ERROR)
			log::warn("Failed to set Doppler factor!");
	}

	void audio_engine::set_speed_of_sound(float speed)
	{
		alSpeedOfSound(speed);

		if (alGetError() != AL_NO_ERROR)
			log::warn("Failed to set speed of sound!");
	}

	audio_state audio_engine::state_of(const std::shared_ptr<audio_source>& source)
	{
		if (!source || !source->m_loaded)
		{
			log::error("Unloaded audio source passed to audio engine!");
			return audio_state::unknown;
		}

		ALint state;
		alGetSourcei(source->m_source_handle, AL_SOURCE_STATE, &state);
		switch (state)
		{
		case AL_STOPPED:	return audio_state::stopped;
		case AL_PLAYING:	return audio_state::playing;
		case AL_PAUSED:		return audio_state::paused;
		default:			return audio_state::unknown;
		}
	}

	void audio_engine::set_debug_logging(bool log)
	{
		s_data.debug_log = log;
	}

	bool audio_engine::is_debug_logging()
	{
		return s_data.debug_log;
	}

	std::shared_ptr<audio_source> audio_engine::load_audio_source_al(raw_buffer buf)
	{
		audio_data data = buf.load<audio_data>();

		ALuint buffer;
		alGenBuffers(1, &buffer);
		alBufferData(buffer, static_cast<ALenum>(data.al_format), data.buffer.as<ALvoid>(), static_cast<ALsizei>(data.buffer.size), static_cast<ALsizei>(data.sample_rate));

		std::shared_ptr<audio_source> result_source = std::make_shared<audio_source>();
		result_source->m_buffer_handle = static_cast<uint32_t>(buffer);
		result_source->m_loaded = true;
		result_source->m_total_duration = data.track_length;

		alGenSources(1, &result_source->m_source_handle);
		alSourcei(result_source->m_source_handle, AL_BUFFER, buffer);

		data.buffer.release();
		buf.release();

		if (alGetError() != AL_NO_ERROR)
			log::error("Failed to setup sound source!");

		return result_source;
	}
}