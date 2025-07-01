#pragma once

#include "audio_source.h"

namespace myro
{
	class audio_engine
	{
	public:
		static void init();

		static audio_source load_audio_source(const std::string& filepath);
		static void play(const audio_source& source);

	private:
		static audio_source load_audio_source_ogg(const std::string& filepath);
		static audio_source load_audio_source_mp3(const std::string& filepath);
	};
}