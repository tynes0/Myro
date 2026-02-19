#pragma once

#include <filesystem>

#include "../../core/buffer.h"

namespace myro
{
    enum class ogg_codec_type
    {
        unknown,
        vorbis,
        opus,
        speex,
        flac
    };

    class ogg_loader
    {
    public:
        static void init(bool debug_log = false);
        static void shutdown(bool debug_log = false);

        static raw_buffer load(const std::filesystem::path& path, bool debug_log = false);

        static ogg_codec_type detect_ogg_codec_robust(const std::filesystem::path& path);
        static bool is_ogg_container(const std::filesystem::path& path);
    };
}