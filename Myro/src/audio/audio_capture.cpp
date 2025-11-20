#include "audio_capture.h"

#include "encoders/wav_encoder.h"
#include "encoders/flac_encoder.h"
#include "../core/log.h"

#include <miniaudio.h>

namespace myro
{
	struct _audio_capture_data
	{
		ma_device device{};
		ma_device_config device_config{};
		bool device_initialized = false;
	};

	class audio_capture_public_write : public audio_capture
	{
	public:
		void write(const short* pcm_frames, size_t frame_count) const
		{
			audio_capture::write(pcm_frames, frame_count);
		}
	};

	namespace 
	{
		void data_callback(ma_device* device, void* output, const void* input, ma_uint32 frame_count)
		{
			if (!input)
				return;
			
			audio_capture_public_write* self = static_cast<audio_capture_public_write*>(device->pUserData);
			static_cast<audio_capture_public_write*>(device->pUserData)->write(static_cast<const short*>(input), frame_count);
		}

		std::shared_ptr<Iencoder> get_encoder(audio_file_format format)
		{
			switch (format)
			{
			case audio_file_format::wav:  return wav_encoder::create();
			case audio_file_format::flac: return flac_encoder::create();
			case audio_file_format::ogg:
			case audio_file_format::mp3:
			case audio_file_format::opus:
			case audio_file_format::spx:
			case audio_file_format::unknown:
				break;
			}

			log::warn("Unsupported or unknown format!");
			return nullptr;
		}
	}

	audio_capture::audio_capture() : m_data(new _audio_capture_data()), m_encoder(nullptr)
	{
	}

	audio_capture::~audio_capture()
	{
		deinit();
		delete m_data;
	}

	bool audio_capture::init(const std::shared_ptr<Iencoder>& encoder)
	{
		if (m_encoder)
			deinit(false);

		if (!encoder || !encoder->initialized())
			return false;

		m_encoder = encoder;

		bool result = init_device();

		if (!result)
			m_encoder = nullptr;

		return result;
	}

	bool audio_capture::init(const std::filesystem::path& output_filepath, uint32_t sample_rate, uint32_t channels)
	{
		if (m_encoder)
			deinit(false);

		audio_file_format format = get_file_format(output_filepath);
		m_encoder = get_encoder(format);

		if (!m_encoder->init(output_filepath, sample_rate, channels))
		{
			log::warn("Encoder initialization failed!");
			m_encoder = nullptr;
			return false;
		}

		bool result = init_device();

		if (!result)
			m_encoder = nullptr;

		return result;
	}

	bool audio_capture::init(const std::filesystem::path& output_filepath, audio_file_format format, uint32_t sample_rate, uint32_t channels)
	{
		if (m_encoder)
			deinit(false);

		m_encoder = get_encoder(format);

		if (!m_encoder->init(output_filepath, sample_rate, channels)) 
		{
			log::warn("Encoder initialization failed!");
			m_encoder = nullptr;
			return false;
		}

		bool result = init_device();

		if (!result)
			m_encoder = nullptr;

		return result;
	}

	void audio_capture::deinit(bool deinit_device)
	{
		if (m_data && deinit_device)
		{
			ma_device_uninit(&m_data->device);
			m_data->device_initialized = false;
		}
		if (m_encoder)
		{
			m_encoder->uninit();
			m_encoder = nullptr;
		}
	}

	bool audio_capture::start() const
	{
		if (!m_encoder || !m_data)
			return false;

		return ma_device_start(&m_data->device) == MA_SUCCESS;
	}

	void audio_capture::stop() const
	{
		if (!m_data)
			return;

		ma_device_stop(&m_data->device);
	}

	void audio_capture::write(const short* pcm_frames, size_t frame_count) const
	{
		if (m_encoder)
			m_encoder->write(pcm_frames, frame_count);
	}

	bool audio_capture::init_device()
	{
		m_data->device_config = ma_device_config_init(ma_device_type_capture);
		m_data->device_config.capture.format = ma_format_s16; // 16-bit short
		m_data->device_config.capture.channels = m_encoder->get_channels();
		m_data->device_config.sampleRate = m_encoder->get_sample_rate();
		m_data->device_config.dataCallback = data_callback;
		m_data->device_config.pUserData = this;

		if (ma_device_init(nullptr, &m_data->device_config, &m_data->device) == MA_SUCCESS)
		{
			m_data->device_initialized = true;
			return true;
		}
		return false;
	}
}

