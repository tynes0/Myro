#include "audio_engine.h"

#include "detail/audio_data.h"
#include "detail/openal_backend.h"

#include "listener.h"
#include "audio_file_format.h"

#include "loaders/ogg_loader.h"
#include "loaders/mp3_loader.h"
#include "loaders/wav_loader.h"
#include "loaders/opus_loader.h"
#include "loaders/speex_loader.h"
#include "loaders/flac_loader.h"

#include "../core/buffer.h"
#include "../core/log.h"
#include "../core/thread_pool.h"

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 5030) 
#endif // _MSC_VER
#include <AL/al.h>
#ifdef _MSC_VER
#pragma warning(pop)
#endif // _MSC_VER

#include <coco.h>

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 26813) // intellisense is just being dramatic...
#endif // _MSC_VER

namespace myro
{
	struct engine_data
	{
		bool active = false;
		bool debug_log = false;
		thread_pool tpool{ 1 };
		std::vector<std::weak_ptr<audio_source>> loaded_sources;
	};

	namespace
	{
		engine_data s_data;

		void init_this_thread_loaders(uint32_t flag)
		{
			if(flag & static_cast<uint32_t>(audio_file_format::ogg))
				ogg_loader::init(s_data.debug_log);
			if(flag & static_cast<uint32_t>(audio_file_format::mp3))
				mp3_loader::init(s_data.debug_log);
			if(flag & static_cast<uint32_t>(audio_file_format::wav))
				wav_loader::init(s_data.debug_log);
			if(flag & static_cast<uint32_t>(audio_file_format::flac))
				flac_loader::init(s_data.debug_log);
			if(flag & static_cast<uint32_t>(audio_file_format::opus))
				opus_loader::init(s_data.debug_log);
			if(flag & static_cast<uint32_t>(audio_file_format::spx))
				speex_loader::init(s_data.debug_log);
		}

		void shutdown_this_thread_loaders(uint32_t flag)
		{
			if (flag & static_cast<uint32_t>(audio_file_format::ogg))
				ogg_loader::shutdown(s_data.debug_log);
			if (flag & static_cast<uint32_t>(audio_file_format::mp3))
				mp3_loader::shutdown(s_data.debug_log);
			if (flag & static_cast<uint32_t>(audio_file_format::wav))
				wav_loader::shutdown(s_data.debug_log);
			if (flag & static_cast<uint32_t>(audio_file_format::flac))
				flac_loader::shutdown(s_data.debug_log);
			if (flag & static_cast<uint32_t>(audio_file_format::opus))
				opus_loader::shutdown(s_data.debug_log);
			if (flag & static_cast<uint32_t>(audio_file_format::spx))
				speex_loader::shutdown(s_data.debug_log);
		}
	}

	void audio_engine::init()
	{
		openal_backend::init(s_data.debug_log);
		listener::init();

		init_this_thread_loaders(
			static_cast<uint32_t>(audio_file_format::ogg) |
			static_cast<uint32_t>(audio_file_format::mp3) |
			static_cast<uint32_t>(audio_file_format::wav) |
			static_cast<uint32_t>(audio_file_format::flac) |
			static_cast<uint32_t>(audio_file_format::opus) |
			static_cast<uint32_t>(audio_file_format::spx));

		s_data.active = true;
	}

	void audio_engine::shutdown()
	{
		shutdown_this_thread_loaders(
			static_cast<uint32_t>(audio_file_format::ogg) |
			static_cast<uint32_t>(audio_file_format::mp3) |
			static_cast<uint32_t>(audio_file_format::wav) |
			static_cast<uint32_t>(audio_file_format::flac) |
			static_cast<uint32_t>(audio_file_format::opus) |
			static_cast<uint32_t>(audio_file_format::spx));
		
		for (auto& wptr : s_data.loaded_sources)
			if (auto sptr = wptr.lock())
				sptr->unload();

		if (s_data.debug_log)
			log::warn("Unloaded {0} sources in shutdown.", s_data.loaded_sources.size());

		s_data.loaded_sources.clear();

		openal_backend::shutdown();

		s_data.active = false;
	}

	bool audio_engine::is_active()
	{
		return s_data.active;
	}

	void audio_engine::set_thread_count(uint32_t count)
	{
		s_data.tpool.set_thread_count(count);
	}

	uint32_t audio_engine::get_thread_count()
	{
		return s_data.tpool.thread_count();
	}

	uint32_t audio_engine::get_max_thread_count()
	{
		return thread_pool::max_thread_count();
	}

	std::shared_ptr<audio_source> audio_engine::load_audio_source(const std::filesystem::path& filepath)
	{
		auto format = get_file_format(filepath);

		if (format == audio_file_format::unknown)
		{
			if (s_data.debug_log)
				log::error("Unknown file format: {}", filepath.extension());
			return nullptr;
		}

		raw_buffer buf;

		coco::timer<coco::time_units::milliseconds> timer;

		switch (format)
		{
		case audio_file_format::ogg: buf = ogg_loader::load(filepath, s_data.debug_log); break;
		case audio_file_format::mp3: buf = mp3_loader::load(filepath, s_data.debug_log); break;
		case audio_file_format::wav: buf = wav_loader::load(filepath, s_data.debug_log); break;
		case audio_file_format::opus:buf = opus_loader::load(filepath, s_data.debug_log); break;
		case audio_file_format::spx: buf = speex_loader::load(filepath, s_data.debug_log); break;
		case audio_file_format::flac:buf = flac_loader::load(filepath, s_data.debug_log); break;
		case audio_file_format::unknown: break;
		}

		timer.stop();
		if (s_data.debug_log)
			log::debug("{0} file loading took: {1}ms", filepath.extension(), timer.get_time());

		if (!buf.data)
		{
			if (s_data.debug_log)
				log::error("Error while loading {} audio source!", filepath.extension());
			return nullptr;
		}

		timer.start();
		auto result = load_audio_source_al(buf);
		timer.stop();

		if (s_data.debug_log)
			log::debug("Audio source loading took: {}ms", timer.get_time());

		s_data.loaded_sources.emplace_back(result);

		return result;
	}

	std::vector<std::shared_ptr<audio_source>> audio_engine::multi_load_audio_source(const std::vector<std::filesystem::path>& filepaths)
	{
		return s_data.tpool.enqueue_bulk([](const std::filesystem::path& filepath) 
			{
				audio_file_format format = get_file_format(filepath);
				uint32_t flags = static_cast<uint32_t>(format);
				if (format == audio_file_format::ogg)
				{
					switch (ogg_loader::detect_ogg_codec_robust(filepath))
					{
					case ogg_codec_type::opus: flags |= static_cast<uint32_t>(audio_file_format::opus); break;
					case ogg_codec_type::speex:flags |= static_cast<uint32_t>(audio_file_format::spx); break;
					case ogg_codec_type::flac: flags |= static_cast<uint32_t>(audio_file_format::flac); break;
					case ogg_codec_type::vorbis:
					case ogg_codec_type::unknown:
						break;
					}
				}

			init_this_thread_loaders(flags);
			auto result = load_audio_source(filepath);
			shutdown_this_thread_loaders(flags);
			return result;
			}, filepaths);
	}

	void audio_engine::unload_audio_source(const std::shared_ptr<audio_source>& source)
	{
		if (!source)
		{
			if (s_data.debug_log)
				log::error("Unloaded audio source passed to audio engine!");
			return;
		}

		source->unload();
	}

	void audio_engine::multi_unload_audio_source(const std::vector<std::shared_ptr<audio_source>>& sources)
	{
		std::ranges::for_each(sources.begin(), sources.end(), [](const std::shared_ptr<audio_source>& ptr) { if (ptr) ptr->unload(); });
	}

	void audio_engine::cleanup_expired_sources()
	{
		auto it = std::ranges::remove_if(s_data.loaded_sources.begin(), s_data.loaded_sources.end(),
			[](const std::weak_ptr<audio_source>& wptr) { return wptr.expired(); });
		s_data.loaded_sources.erase(it.begin(), s_data.loaded_sources.end());
	}

	void audio_engine::play(const std::shared_ptr<audio_source>& source)
	{
		if (!source || !source->m_loaded)
		{
			if (s_data.debug_log)
				log::error("Unloaded audio source passed to audio engine!");
			return;
		}

		alSourcePlay(source->m_source_handle);
	}

	void audio_engine::stop(const std::shared_ptr<audio_source>& source)
	{
		if (!source || !source->m_loaded)
		{
			if (s_data.debug_log)
				log::error("Unloaded audio source passed to audio engine!");
			return;
		}

		alSourceStop(source->m_source_handle);
	}

	void audio_engine::pause(const std::shared_ptr<audio_source>& source)
	{
		if (!source || !source->m_loaded)
		{
			if (s_data.debug_log)
				log::error("Unloaded audio source passed to audio engine!");
			return;
		}

		alSourcePause(source->m_source_handle);
	}

	void audio_engine::rewind(const std::shared_ptr<audio_source>& source)
	{
		if (!source || !source->m_loaded)
		{
			if (s_data.debug_log)
				log::error("Unloaded audio source passed to audio engine!");
			return;
		}

		alSourceRewind(source->m_source_handle);
	}

	void audio_engine::seek(const std::shared_ptr<audio_source>& source, float seconds)
	{
		if (!source || !source->m_loaded)
		{
			if (s_data.debug_log)
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
			if (s_data.debug_log)
				log::warn("Failed to set Doppler factor!");
	}

	void audio_engine::set_speed_of_sound(float speed)
	{
		alSpeedOfSound(speed);

		if (alGetError() != AL_NO_ERROR)
			if (s_data.debug_log)
				log::warn("Failed to set speed of sound!");
	}

	audio_state audio_engine::state_of(const std::shared_ptr<audio_source>& source)
	{
		if (!source || !source->m_loaded)
		{
			if (s_data.debug_log)
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

		if (!data.buffer)
		{
			if (s_data.debug_log)
				log::error("Failed to setup audio source!");
			return nullptr;
		}

		ALuint buffer;
		alGenBuffers(1, &buffer);
		alBufferData(buffer, static_cast<ALenum>(data.al_format), data.buffer.as<ALvoid>(), static_cast<ALsizei>(data.buffer.size), static_cast<ALsizei>(data.sample_rate));

		std::shared_ptr<audio_source> result_source = std::make_shared<audio_source>();
		result_source->m_buffer_handle = static_cast<uint32_t>(buffer);
		result_source->m_loaded = true;
		result_source->m_total_duration = data.track_length;

		alGenSources(1, &result_source->m_source_handle);
		alSourcei(result_source->m_source_handle, AL_BUFFER, static_cast<ALint>(buffer));

		data.buffer.release();
		buf.release();

		if (alGetError() != AL_NO_ERROR)
			if (s_data.debug_log)
				log::error("Failed to setup audio source!");

		return result_source;
	}
}

#ifdef _MSC_VER
#pragma warning(pop)
#endif // _MSC_VER