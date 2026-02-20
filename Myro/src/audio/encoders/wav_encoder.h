#pragma once

#include "iencoder.h"
#include <memory>

namespace myro
{
	class wav_encoder : public IEncoder
	{
	public:
		static std::shared_ptr<wav_encoder> create() { return std::make_shared<wav_encoder>(); }

		bool init(const std::filesystem::path& output_filepath, unsigned int sample_rate, unsigned int channels) override;
		void deinit() override;

		[[nodiscard]] bool initialized() const override;

		[[nodiscard]] uint32_t get_sample_rate() const override { return m_sample_rate; }
		[[nodiscard]] uint16_t get_channels() const override { return m_channels; }
		[[nodiscard]] uint16_t get_bits_per_sample() const override { return m_bits_per_sample; }
	private:
		void write(const short* pcm_frames, size_t frame_count) override;
		void write_header_placeholder();
		void update_header();
	private:
		friend class audio_capture;

		FILE* m_out_file = nullptr;
		uint32_t m_data_bytes_written = 0;
		uint32_t m_sample_rate = 0;
		uint16_t m_channels = 0;
		uint16_t m_bits_per_sample = 16;
		char m_header[44]{0};
	};
}