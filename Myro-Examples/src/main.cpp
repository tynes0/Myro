#include <myro.h>

#include <chrono>
#include <coco.h>

int main()
{
	myro::audio_engine::init();

	myro::audio_capture capture;
	capture.init("assets/test.ogg", 44100, 2);

	if (capture.start())
	{
		std::this_thread::sleep_for(std::chrono::seconds(3));
		capture.stop();
	}

	capture.deinit();

	myro::audio_engine::shutdown();
}