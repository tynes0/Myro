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
		void fclose_checked(FILE* stream);
		void fseek_checked(FILE* stream, long offset, int origin);
		void fwrite_checked(void const* buf, size_t elem_size, size_t elem_count, FILE* stream);
	}
}