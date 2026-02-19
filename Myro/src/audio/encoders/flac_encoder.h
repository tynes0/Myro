#pragma once

#include "iencoder.h"

namespace myro
{
	struct _flac_encoder_data;

	class flac_encoder : public Iencoder
	{
	public:
		static std::shared_ptr<flac_encoder> create() { return std::make_shared<flac_encoder>(); }

		flac_encoder();
		~flac_encoder();

		bool init(const std::filesystem::path& output_filepath, unsigned int sample_rate, unsigned int channels) override;
		void uninit() override;

		virtual bool initialized() const override;

		uint32_t get_sample_rate() const override;
		uint16_t get_channels() const override;
		uint16_t get_bits_per_sample() const override;
	private:
		virtual void write(const short* pcm_frames, size_t frame_count) override;
		
		friend class audio_capture;

		_flac_encoder_data* m_data;
	};
}