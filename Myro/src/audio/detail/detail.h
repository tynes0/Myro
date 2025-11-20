#pragma once

#include <string>
#include <cstdint>
#include <filesystem>

namespace myro
{
	namespace detail
	{
		void display_file_info(const std::string& filename, int64_t channels, int64_t sample_rate, uint64_t size);
		FILE* open_file(const std::filesystem::path& path, const char* mode);
	}
}