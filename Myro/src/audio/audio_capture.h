#pragma once

#include "encoders/iencoder.h"
#include "audio_file_format.h"
#include <memory>

namespace myro
{
	struct _audio_capture_data;

	class audio_capture
	{
	public:
		audio_capture();
		~audio_capture();

		audio_capture(const audio_capture&) = delete;
		audio_capture& operator=(const audio_capture&) = delete;
		audio_capture(audio_capture&&) = delete;
		audio_capture& operator=(audio_capture&&) = delete;

		bool init(const std::shared_ptr<IEncoder>& encoder);
		bool init(const std::filesystem::path& output_filepath, uint32_t sample_rate, uint32_t channels);
		bool init(const std::filesystem::path& output_filepath, audio_file_format format, uint32_t sample_rate, uint32_t channels);
		void deinit(bool deinit_device = true);

		bool start() const;
		void stop() const;

	protected:
		void write(const short* pcm_frames, size_t frame_count) const;
	private:
		bool init_device();
		
		_audio_capture_data* m_data;
		std::shared_ptr<IEncoder> m_encoder;
	};
}