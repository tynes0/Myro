#include <myro.h>

#include <chrono>
#include <thread>

#include <coco/coco.h>

int main()
{
	//myro::audio_engine::set_debug_logging(true);

	myro::audio_engine::init();

	myro::audio_engine::set_thread_count(6);

	std::vector<std::filesystem::path> files{ "assets/mp3_test.mp3", "assets/wav_test.wav", "assets/ogg_vorbis_test.ogg", "assets/opus_test.opus", "assets/flac_test.flac", "assets/ogg_flac_test.ogg" };

	coco::timer<coco::time_units::milliseconds> timer;

	//std::shared_ptr<myro::audio_source> mp3_test = myro::audio_source::load_from_file("assets/mp3_test.mp3", true);
	//std::shared_ptr<myro::audio_source> wav_test = myro::audio_source::load_from_file("assets/wav_test.wav", true);
	//std::shared_ptr<myro::audio_source> ogg_vorbis_test = myro::audio_source::load_from_file("assets/ogg_vorbis_test.ogg", true);
	//std::shared_ptr<myro::audio_source> opus_test = myro::audio_source::load_from_file("assets/opus_test.opus", true);
	//std::shared_ptr<myro::audio_source> flac_test = myro::audio_source::load_from_file("assets/flac_test.flac", true);
	//std::shared_ptr<myro::audio_source> ogg_flac_test = myro::audio_source::load_from_file("assets/ogg_flac_test.ogg", true);

	std::vector<std::shared_ptr<myro::audio_source>> tests = myro::audio_engine::multi_load_audio_source(files);

	timer.stop();
	myro::log::error("Total loading took: {0}ms", timer.get_time());

	myro::audio_engine::shutdown();

	//std::shared_ptr<myro::audio_source> speex_test = myro::audio_source::load_from_file("assets/spx_test.spx", true);
	//myro::audio_engine::play(speex_test);

	//std::this_thread::sleep_for(std::chrono::seconds(5));

	//myro::audio_capture cap;
	//cap.init("testfile.flac", 44100, 2);
	//
	//cap.start();
	//
	//std::this_thread::sleep_for(std::chrono::seconds(5));
	//
	//cap.stop();
	//
	//cap.uninit();
}