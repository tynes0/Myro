#include "audio_engine.h"

#include "../core/raw_buffer.h"
#include "../core/log.h"

#include <AL/al.h>
#include <AL/alext.h>
#include <alc/alcmain.h>
#include <alhelpers.h>

#define MINIMP3_IMPLEMENTATION
#include <minimp3.h>
#include <minimp3_ex.h>

#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>

#define INITIAL_SCRATCH_BUFFER_SIZE 10 * 1024 * 1024 // 10 mb

namespace myro
{
	struct engine_data
	{
		ALCdevice* audio_device = nullptr;
		mp3dec_t mp3_decoder;

		raw_buffer audio_scratch_buffer;
	};


	static engine_data s_data;

	void audio_engine::init()
	{
		if (InitAL(s_data.audio_device, nullptr, 0) != 0)
			logger::error("Audio device error!");

		mp3dec_init(&s_data.mp3_decoder);

		s_data.audio_scratch_buffer.allocate(INITIAL_SCRATCH_BUFFER_SIZE);


	}

}