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

#define MYRO_UNUSED(x) ((void)x)

#define _MYRO_STRINGIZE_(x) #x
#define MYRO_STRINGIZE(x) _MYRO_STRINGIZE_(x)

#define MYRO_EXPAND(x) x

#define _MYRO_CONCATENATE_(a, b) a ## b
#define MYRO_CONCATENATE(a, b)  _MYRO_CONCATENATE_(a, b)

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