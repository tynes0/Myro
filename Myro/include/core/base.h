#pragma once

// Platform Detection
#if defined(_WIN32)
	#if defined(_WIN64)
		#define MYRO_PLATFORM_WINDOWS 1
	#else
		#error "x86 Builds are not supported (for now)!"
	#endif
#elif defined(__APPLE__) || defined(__MACH__)
	#include <TargetConditionals.h>
	#if TARGET_IPHONE_SIMULATOR
		#error "IOS simulator is not supported (for now)!"
	#elif TARGET_OS_IPHONE
		#define MYRO_PLATFORM_IOS 1
		//#error "IOS is not supported (for now)!"
	#elif TARGET_OS_MAC
		#define MYRO_PLATFORM_MACOS 1
		//#error "MacOS is not supported (for now)!"
	#else
		#error "Unknown Apple platform!"
	#endif
#elif defined(__ANDROID__)
	#define MYRO_PLATFORM_ANDROID 1
	//#error "Android is not supported (for now)!"
#elif defined(__linux__)
	#define MYRO_PLATFORM_LINUX 1
	//#error "Linux is not supported (for now)!"
#else
	#error "Unknown platform!"
#endif

#define MYRO_UNUSED(x) ((void)(x))

#define MYRO_STRINGIZE_(x) #x
#define MYRO_STRINGIZE(x) MYRO_STRINGIZE_(x)

#define MYRO_EXPAND(x) x

#define MYRO_CONCATENATE_(a, b) a ## b
#define MYRO_CONCATENATE(a, b)  MYRO_CONCATENATE_(a, b)

// Debug Break
#if defined(MYRO_PLATFORM_WINDOWS)
	#define MYRO_DEBUGBREAK() __debugbreak()
#elif defined(MYRO_PLATFORM_LINUX) || defined(MYRO_PLATFORM_MACOS)
	#include <signal.h>
	#define MYRO_DEBUGBREAK() raise(SIGTRAP)
#else
	#define MYRO_DEBUGBREAK() ((void)0)
#endif

#ifdef MYRO_DEBUG
#define MYRO_ENABLE_ASSERTS  // Myro asserts enabled
#endif

#if defined(__GNUC__) || defined(__clang__)
	#define MYRO_ANONYMOUS_STRUCT __extension__ struct
#else
	#define MYRO_ANONYMOUS_STRUCT struct
#endif

namespace myro
{
	template <typename T, uint64_t Size>
	concept BitSizeType = std::is_integral_v<T> && ((sizeof(T) * 8) > Size);
	
	namespace constants
	{
		template <uint64_t Exponent, typename T = uint8_t>
			requires BitSizeType<T, Exponent>
		inline constexpr T bit = static_cast<T>(1 << Exponent);

		inline constexpr size_t initial_scratch_buffer_size = (static_cast<size_t>(10) * 1024 * 1024); // 10 MB
		inline constexpr size_t pcm_flt_buffer_size = 1024; // I am not sure about that
		inline constexpr size_t pcm_buffer_size = 1024; // I am not sure about that
	}
}