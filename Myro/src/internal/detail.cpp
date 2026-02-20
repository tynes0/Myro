#include "detail.h"

#include "core/log.h"

#include <cstdio>
#include <cerrno>
#include <system_error>

namespace myro::detail
{
	void display_file_info(const std::string& filename, int64_t channels, int64_t sample_rate, uint64_t size)
	{
		log::info("~~~~ FILE INFO ~~~~");
		log::info("File: {}", filename);
		log::info("Channels: {}", channels);
		log::info("Sample rate: {}", sample_rate);
		log::info("Expected Size: {}", size);
	}

    FILE* open_file(const std::filesystem::path& path, const char* mode) 
    {
        if (!mode)
            return nullptr;

#if defined(_WIN32)
    #if defined(_MSC_VER) || defined(__MINGW64__) || !defined(__STRICT_ANSI__)
        bool has_r = false, has_w = false, has_a = false;
        bool has_plus = false, has_b = false;

        for (const char* p = mode; *p; ++p)
        {
            switch (*p) 
            {
            case 'r': has_r = true; break;
            case 'w': has_w = true; break;
            case 'a': has_a = true; break;
            case '+': has_plus = true; break;
            case 'b': has_b = true; break;
            default:
                errno = EINVAL;
                return nullptr;
            }
        }

        int base_count = static_cast<int>(has_r) + static_cast<int>(has_w) + static_cast<int>(has_a);
        if (base_count != 1) 
        {
            errno = EINVAL;
            return nullptr;
        }

        std::wstring wmode;
        if (has_r) wmode += L"r";
        if (has_w) wmode += L"w";
        if (has_a) wmode += L"a";
        if (has_plus) wmode += L"+";
        if (has_b)    wmode += L"b";

        return _wfopen(path.c_str(), wmode.c_str());

    #else // defined(_MSC_VER) || defined(__MINGW64__) || !defined(__STRICT_ANSI__)
        std::string ansi_path = path.string();
        return fopen(ansi_path.c_str(), mode);
    #endif // defined(_MSC_VER) || defined(__MINGW64__) || !defined(__STRICT_ANSI__)
#else // defined(_WIN32)
        std::string utf8_path(path.u8string().begin(), path.u8string().end());
        return fopen(utf8_path.c_str(), mode);
#endif // defined(_WIN32)
    }

    void fclose_checked(FILE* stream)
    {
        if (stream != nullptr && std::fclose(stream) != 0)
        {
            std::error_code ec(errno, std::system_category());
            log::error("Failed to close file. Reason: {}", ec.message());
        }
    }
    
    void fseek_checked(FILE* stream, long offset, int origin)
    {
        if (std::fseek(stream, offset, origin) != 0)
        {
            std::error_code ec(errno, std::system_category());
            
            const char* origin_str = (origin == SEEK_SET) ? "SEEK_SET" :
                                     (origin == SEEK_CUR) ? "SEEK_CUR" :
                                     (origin == SEEK_END) ? "SEEK_END" : "UNKNOWN";
    
            log::error("Failed to seek file (offset: {}, origin: {}). Reason: {}", 
                             offset, origin_str, ec.message());
        }
    }
    
    void fwrite_checked(void const* buf, size_t elem_size, size_t elem_count, FILE* stream)
    {
        size_t written = std::fwrite(buf, elem_size, elem_count, stream);
        
        if (written < elem_count)
        {
            std::error_code ec(errno, std::system_category());
            log::error("Failed to write to file. Expected {} elements, wrote {}. Reason: {}",  elem_count, written, ec.message());
        }
    }
}
