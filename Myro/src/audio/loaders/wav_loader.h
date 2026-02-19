#pragma once

#include <filesystem>

#include "../../core/buffer.h"

namespace myro
{
    class wav_loader
    {
    public:
        static void init();
        static void shutdown();
        static raw_buffer load(const std::filesystem::path& filepath);
    };
}