#pragma once

#include <filesystem>

#include "core/buffer.h"

namespace myro
{
    class speex_loader
    {
    public:
        static void init();
        static void shutdown();
        static raw_buffer load(const std::filesystem::path& filepath);
        static raw_buffer load_ogg_speex(const std::filesystem::path& filepath);
    };
}