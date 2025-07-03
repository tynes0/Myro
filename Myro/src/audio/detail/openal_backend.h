#pragma once

#include <AL/al.h>
#include <AL/alc.h>

#include <cstdint>

namespace myro
{
	class openal_backend
	{
	public:
		static bool init(bool debug_log = false);
		static void shutdown();

		static ALenum get_openAL_format(uint32_t channels);
		static ALCdevice* get_device();

		static void display_audio_device_info();
	};
}