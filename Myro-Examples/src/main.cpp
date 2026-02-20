#include <myro.h>

#include <chrono>
#include <coco.h>


int main()
{
	myro::audio_engine::init();
	myro::log::set_logger_activity(myro::log::level_critical | myro::log::level_error | myro::log::level_warn | myro::log::level_info | myro::log::level_trace);
	myro::audio_engine::set_thread_count(6);

	std::vector<std::filesystem::path> files{ "assets/mp3_test.mp3", "assets/wav_test.wav", "assets/ogg_vorbis_test.ogg", "assets/opus_test.opus", "assets/flac_test.flac", "assets/ogg_flac_test.ogg" };

	coco::timer<coco::time_units::milliseconds> timer;
	std::vector<std::shared_ptr<myro::audio_source>> tests = myro::audio_engine::multi_load_audio_source(files);
	timer.stop();

	myro::log::error("Total loading took: {0}ms", timer.get_time());

	myro::audio_engine::play(tests[1]);

	while (myro::audio_engine::state_of(tests[1]) == myro::audio_state::playing)
	{
		float current_dur = tests[1]->get_current_duration();
		myro::log::info("Sound playing: {}", current_dur);
	}

	myro::audio_engine::shutdown();
}