#pragma once

#include <filesystem>

#include "core/buffer.h"

namespace myro
{
    enum class ogg_codec_type : uint8_t
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
        static void init();
        static void shutdown();

        static raw_buffer load(const std::filesystem::path& path);

        static ogg_codec_type detect_ogg_codec_robust(const std::filesystem::path& path);
        static bool is_ogg_container(const std::filesystem::path& path);
    };
}