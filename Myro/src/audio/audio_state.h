#pragma once

namespace myro
{
	enum class audio_state : uint8_t
	{
		unknown, 
		stopped,
		playing,
		paused
	};
}