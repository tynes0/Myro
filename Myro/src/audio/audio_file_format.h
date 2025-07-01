#pragma once

#include <string>

namespace myro
{
	enum class audio_file_format
	{
		none = 0,
		ogg,
		mp3
	};

	audio_file_format get_file_format(const std::string& filepath);
}