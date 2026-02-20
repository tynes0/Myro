#pragma once

#include <memory>

#include "audio_source.h"

namespace myro
{
	enum class audio_filter : uint8_t
	{
		unknown = 0,
		low_pass_filter,
		high_pass_filter
	};

	class audio_filter_manager
	{
	public:
		static void apply_low_pass_filter(const std::shared_ptr<audio_source>& source, float gain, float gainHF);
		static void apply_high_pass_filter(const std::shared_ptr<audio_source>& source, float gain, float gainLF);
		static void remove_filter(const std::shared_ptr<audio_source>& source, audio_filter type);
	};
}