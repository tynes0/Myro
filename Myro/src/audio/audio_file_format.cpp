#include "audio_file_format.h"

#include <filesystem>

namespace myro
{
    audio_file_format get_file_format(const std::filesystem::path& filepath)
    {
        std::filesystem::path p_extension = filepath.extension();
        std::string extension = p_extension.string();

        if (extension == ".ogg") return audio_file_format::ogg;
        if (extension == ".mp3") return audio_file_format::mp3;
        if (extension == ".wav") return audio_file_format::wav;
        if (extension == ".flac") return audio_file_format::flac;
        if (extension == ".opus") return audio_file_format::opus;
        if (extension == ".spx") return audio_file_format::spx;

        return audio_file_format::unknown;
    }

    audio_file_format get_file_format(const std::string& filepath)
    {
        return get_file_format(std::filesystem::path(filepath));
    }
}

