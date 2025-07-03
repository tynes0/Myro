#pragma once

#include <filesystem>

#include "../../core/buffer.h"

namespace myro
{
    class mp3_loader
    {
    public:
        static void init();
        static raw_buffer load(const std::filesystem::path& path, bool debug_log = false);
    };
}