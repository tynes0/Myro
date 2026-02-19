#pragma once

#include <string>
#include <filesystem>

#include "../core/base.h"

namespace myro
{
	enum class audio_file_format : uint8_t
	{
		unknown = constants::bit<0>,
		ogg		= constants::bit<1>,
		mp3		= constants::bit<2>,
		wav		= constants::bit<3>,
		flac	= constants::bit<4>,
		opus	= constants::bit<5>,
		spx		= constants::bit<6>
	};

	audio_file_format get_file_format(const std::filesystem::path& filepath);
	audio_file_format get_file_format(const std::string& filepath);
}