#pragma once

#include <string>
#include <filesystem>

namespace myro
{
	enum class audio_file_format
	{
		unknown = 0,
		ogg,
		mp3,
		wav,
		flac,
		opus,
		spx
	};

	audio_file_format get_file_format(const std::filesystem::path& filepath);
	audio_file_format get_file_format(const std::string& filepath);
}