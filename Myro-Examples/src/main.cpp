#include <myro.h>

#include <chrono>
#include <coco.h>


int main()
{
	myro::audio_engine::init();
	myro::log::set_logger_activity(false);
	myro::audio_engine::set_thread_count(6);

	std::vector<std::filesystem::path> files{ "assets/mp3_test.mp3", "assets/wav_test.wav", "assets/ogg_vorbis_test.ogg", "assets/opus_test.opus", "assets/flac_test.flac", "assets/ogg_flac_test.ogg" };

	coco::timer<coco::time_units::milliseconds> timer;
	std::vector<std::shared_ptr<myro::audio_source>> tests = myro::audio_engine::multi_load_audio_source(files);
	timer.stop();

	myro::log::set_logger_activity(true);
	myro::log::error("Total loading took: {0}ms", timer.get_time());

	myro::audio_engine::shutdown();
}