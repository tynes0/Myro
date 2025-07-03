#pragma once

#include <filesystem>

#include "../../core/buffer.h"

namespace myro
{
    class wav_loader
    {
    public:
        static void init();
        static raw_buffer load(const std::filesystem::path& filepath, bool debug_log = false);
    };
}