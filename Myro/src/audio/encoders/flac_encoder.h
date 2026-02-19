#pragma once

#include "IEncoder.h"

namespace myro
{
	struct _flac_encoder_data;

	// NOLINTNEXTLINE(cppcoreguidelines-special-member-functions)
	class flac_encoder : public IEncoder
	{
	public:
		static std::shared_ptr<flac_encoder> create() { return std::make_shared<flac_encoder>(); }

		flac_encoder();
		~flac_encoder() override;

		bool init(const std::filesystem::path& output_filepath, unsigned int sample_rate, unsigned int channels) override;
		void deinit() override;

		[[nodiscard]] bool initialized() const override;

		[[nodiscard]] uint32_t get_sample_rate() const override;
		[[nodiscard]] uint16_t get_channels() const override;
		[[nodiscard]] uint16_t get_bits_per_sample() const override;
	private:
		void write(const short* pcm_frames, size_t frame_count) override;
		
		friend class audio_capture;

		_flac_encoder_data* m_data;
	};
}