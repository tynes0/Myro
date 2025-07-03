#pragma once

#include <cstdint>
#include <string>

namespace myro
{
	namespace detail
	{
		void display_file_info(const std::string& filename, int64_t channels, int64_t sample_rate, uint64_t size);
	}
}