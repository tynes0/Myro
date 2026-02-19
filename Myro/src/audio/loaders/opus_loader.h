#pragma once

#include <filesystem>

#include "../../core/buffer.h"

namespace myro
{
    class opus_loader
    {
    public:
        static void init();
        static void shutdown();
        static raw_buffer load(const std::filesystem::path& filepath);
        static raw_buffer load_ogg_opus(const std::filesystem::path& filepath);
    };
}