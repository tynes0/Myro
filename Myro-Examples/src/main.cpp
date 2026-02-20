#include <myro.h>

#include <chrono>
#include <coco.h>

int main()
{
	myro::audio_engine::init();
	myro::log::set_logger_activity(myro::log::level_error | myro::log::level_info | myro::log::level_warn);

	myro::audio_capture capture;
	capture.init("assets/test.opus", 48000, 2);

	if (capture.start())
	{
		std::this_thread::sleep_for(std::chrono::seconds(3));
		capture.stop();
	}
	else
	{
		myro::log::error("encoder initializing failed!");
	}

	capture.deinit();

	myro::audio_engine::shutdown();
}
