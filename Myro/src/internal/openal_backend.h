#pragma once

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 5030)
#endif // _MSC_VER

#include <AL/al.h>
#include <AL/alc.h>

#ifdef _MSC_VER
#pragma warning(pop)
#endif // _MSC_VER

#include <cstdint>

namespace myro
{
	class openal_backend
	{
	public:
		static bool init();
		static void shutdown();

		static ALenum get_openAL_format(uint32_t channels, uint32_t bits_per_sample = 16);
		static ALCdevice* get_device();
	};
}