#include "audio_file_format.h"

#include <filesystem>

namespace myro
{
    audio_file_format get_file_format(const std::string& filepath)
    {
        std::filesystem::path path = filepath;
        std::filesystem::path p_extension = path.extension();
        std::string extension = p_extension.string();

        if (extension == ".ogg") return audio_file_format::ogg;
        if (extension == ".mp3") return audio_file_format::mp3;

        return audio_file_format::none;
    }
}

