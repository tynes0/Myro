#include <myro.h>

int main()
{
	myro::audio_engine::set_debug_logging(true);

	myro::audio_engine::init();

	std::shared_ptr<myro::audio_source> water_mp3 = myro::audio_source::load_from_file("assets/water.mp3", true);
	std::shared_ptr<myro::audio_source> water_wav = myro::audio_source::load_from_file("assets/water.wav", true);
	//std::shared_ptr<myro::audio_source> water_ogg = myro::audio_source::load_from_file("assets/water.ogg", true);
	
	water_mp3->set_position({ 1.0f, 0.0f, 0.0f });
	water_wav->set_position({ 0.0f, 1.0f, 0.0f });
	//water_ogg->set_position({ 0.0f, 0.0f, 1.0f });

	myro::audio_engine::play(water_mp3);


}