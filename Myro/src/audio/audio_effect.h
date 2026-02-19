#pragma once

#include <memory>

#include "audio_source.h"

namespace myro
{
	enum class audio_effect
	{
		unknown = 0,
		reverb,
		echo,
		chorus,
		distortion,
		flanger,
		equalizer,
		frequency_shifter,
		autowah,
		ring_modulator
	};

	class audio_effect_manager
	{
	public:
		static void apply_reverb(const std::shared_ptr<audio_source>& source, float decay_time, float density);
		static void apply_echo(const std::shared_ptr<audio_source>& source, float delay, float damping);
		static void apply_chorus(const std::shared_ptr<audio_source>& source, float rate, float depth, float feedback);
		static void apply_distortion(const std::shared_ptr<audio_source>& source, float edge, float gain, float lowpass_cutoff);
		static void apply_flanger(const std::shared_ptr<audio_source>& source, float rate, float depth, float feedback);
		static void apply_equalizer(const std::shared_ptr<audio_source>& source, float low_gain, float mid_gain, float high_gain);
		static void apply_frequency_shifter(const std::shared_ptr<audio_source>& source, float frequency, int direction);
		static void apply_autowah(const std::shared_ptr<audio_source>& source, float attack_time, float release_time, float resonance);
		static void apply_ring_modulator(const std::shared_ptr<audio_source>& source, float frequency, float highpass_cutoff);

		static void remove_effect(const std::shared_ptr<audio_source>& source, audio_effect type);
	};
}