#pragma once
#include <chrono>
#include <alloca.h>
#include <cstdarg>
#include <ym_types.h>

namespace ym::log
{
    static inline decltype(std::chrono::high_resolution_clock::now()) start_of_program;
    void
    init()
    {
        start_of_program = std::chrono::high_resolution_clock::now();
    }

    template<ym::size_t size_of_filename>
    void
    log(std::FILE* file,
        const char* type,
        const char* filepath,
        const char* func,
        const int line,
        const char* fmt,
        ...)
    {
        using ms = std::chrono::milliseconds::period;
        namespace clock = std::chrono;

        const auto now = clock::high_resolution_clock::now();
        const auto diff = clock::duration<float, ms>(now - start_of_program).count();

        va_list args1;
        va_start(args1, fmt);
        va_list args2;
        va_copy(args2, args1);
        ym::size_t size = 1 + std::vsnprintf(nullptr, 0, fmt, args1);
        char* buffer = static_cast<char*>(alloca(size));
        va_end(args1);
        std::vsnprintf(buffer, size, fmt, args2);
        va_end(args2);

        const char* filename = std::strrchr(filepath, '/');
        filename++;

        std::fprintf(file, "[%.5f][%-5s]: %-10s: %-25s:%4d: %s\n",
                     diff, type,
                     filename, func,
                     line, buffer);
    }
}

#define YM_INIT_LOGGING \
ym::log::impl::start_logging();

#define YM_WARN(fmt, ...) \
ym::log::impl::log_info<sizeof(__FILE__)>(stderr, "WARN", __FILE__, __func__, __LINE__, fmt, ##__VA_ARGS__);

#define YM_INFO(fmt, ...) \
ym::log::impl::log_info<sizeof(__FILE__)>(stdout, "INFO", __FILE__, __func__, __LINE__, fmt, ##__VA_ARGS__);

#define YM_DEBUG(fmt, ...) \
ym::log::impl::log_info<sizeof(__FILE__)>(stdout, "DEBUG", __FILE__, __func__, __LINE__, fmt, ##__VA_ARGS__);
