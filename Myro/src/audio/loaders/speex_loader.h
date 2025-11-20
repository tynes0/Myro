#pragma once

#include <filesystem>

#include "../../core/buffer.h"

namespace myro
{
    class speex_loader
    {
    public:
        static void init(bool debug_log = false);
        static void shutdown(bool debug_log = false);
        static raw_buffer load(const std::filesystem::path& filepath, bool debug_log = false);
        static raw_buffer load_ogg_speex(const std::filesystem::path& filepath, bool debug_log = false);
    };
}