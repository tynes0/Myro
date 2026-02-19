#include "audio_effect.h"

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:  5030)
#endif // _MSC_VER
#define AL_ALEXT_PROTOTYPES
#include <AL/al.h>
#include <AL/efx.h>
#ifdef _MSC_VER
#pragma warning(pop)
#endif // _MSC_VER

#include <unordered_map>

// NOLINTBEGIN(readability-suspicious-call-argument)

namespace myro
{
	struct effect_data
	{
		ALuint slot;
		ALuint effect;
		audio_effect type;
	};

	struct effect_manager_data
	{
		std::unordered_map<ALuint, effect_data> effect_datas;
	};

	namespace { effect_manager_data s_data; }

	void audio_effect_manager::apply_reverb(const std::shared_ptr<audio_source>& source, float decay_time, float density)
	{
		if (!source)
		{
			log::error("Unloaded audio source passed to audio effect manager!");
			return;
		}

		ALuint effect;
		alGenEffects(1, &effect);
		alEffecti(effect, AL_EFFECT_TYPE, AL_EFFECT_REVERB);
		alEffectf(effect, AL_REVERB_DECAY_TIME, decay_time);
		alEffectf(effect, AL_REVERB_DENSITY, density);

		ALuint effect_slot;
		alGenAuxiliaryEffectSlots(1, &effect_slot);
		alAuxiliaryEffectSloti(effect_slot, AL_EFFECTSLOT_EFFECT, static_cast<ALint>(effect));
		alSource3i(source->m_source_handle, AL_AUXILIARY_SEND_FILTER, static_cast<ALint>(effect_slot), 0, AL_FILTER_NULL);

		if (alGetError() != AL_NO_ERROR)
		{
			log::warn("Error attaching reverb effect to source!");
		}
		else
		{
			s_data.effect_datas[source->m_source_handle] = {
				.slot = effect_slot,
				.effect = effect,
				.type = audio_effect::reverb
			};
		}
	}

	void audio_effect_manager::apply_echo(const std::shared_ptr<audio_source>& source, float delay, float damping)
	{
		if (!source)
		{
			log::error("Unloaded audio source passed to audio effect manager!");
			return;
		}

		ALuint effect;
		alGenEffects(1, &effect);
		alEffecti(effect, AL_EFFECT_TYPE, AL_EFFECT_ECHO);
		alEffectf(effect, AL_ECHO_DELAY, delay);
		alEffectf(effect, AL_ECHO_DAMPING, damping);

		ALuint effect_slot;
		alGenAuxiliaryEffectSlots(1, &effect_slot);
		alAuxiliaryEffectSloti(effect_slot, AL_EFFECTSLOT_EFFECT, static_cast<ALint>(effect));
		alSource3i(source->m_source_handle, AL_AUXILIARY_SEND_FILTER, static_cast<ALint>(effect_slot), 0, AL_FILTER_NULL);

		if (alGetError() != AL_NO_ERROR)
		{
			log::warn("Error attaching echo effect to source!");
		}
		else
		{
			s_data.effect_datas[source->m_source_handle] = {
				.slot = effect_slot,
				.effect = effect,
				.type = audio_effect::echo
			};
		}
	}

	void audio_effect_manager::apply_chorus(const std::shared_ptr<audio_source>& source, float rate, float depth, float feedback)
	{
		if (!source)
		{
			log::error("Unloaded audio source passed to audio effect manager!");
			return;
		}

		ALuint effect;
		alGenEffects(1, &effect);
		alEffecti(effect, AL_EFFECT_TYPE, AL_EFFECT_CHORUS);
		alEffectf(effect, AL_CHORUS_RATE, rate);
		alEffectf(effect, AL_CHORUS_DEPTH, depth);
		alEffectf(effect, AL_CHORUS_FEEDBACK, feedback);

		ALuint effect_slot;
		alGenAuxiliaryEffectSlots(1, &effect_slot);
		alAuxiliaryEffectSloti(effect_slot, AL_EFFECTSLOT_EFFECT, static_cast<ALint>(effect));
		alSource3i(source->m_source_handle, AL_AUXILIARY_SEND_FILTER, static_cast<ALint>(effect_slot), 0, AL_FILTER_NULL);

		if (alGetError() != AL_NO_ERROR)
		{
			log::warn("Error attaching chorus effect to source!");
		}
		else
		{
			s_data.effect_datas[source->m_source_handle] = {
				.slot = effect_slot,
				.effect = effect,
				.type = audio_effect::chorus
			};
		}
	}

	void audio_effect_manager::apply_distortion(const std::shared_ptr<audio_source>& source, float edge, float gain, float lowpass_cutoff)
	{
		if (!source)
		{
			log::error("Unloaded audio source passed to audio effect manager!");
			return;
		}

		ALuint effect;
		alGenEffects(1, &effect);
		alEffecti(effect, AL_EFFECT_TYPE, AL_EFFECT_DISTORTION);
		alEffectf(effect, AL_DISTORTION_EDGE, edge);
		alEffectf(effect, AL_DISTORTION_GAIN, gain);
		alEffectf(effect, AL_DISTORTION_LOWPASS_CUTOFF, lowpass_cutoff);

		ALuint effect_slot;
		alGenAuxiliaryEffectSlots(1, &effect_slot);
		alAuxiliaryEffectSloti(effect_slot, AL_EFFECTSLOT_EFFECT, static_cast<ALint>(effect));
		alSource3i(source->m_source_handle, AL_AUXILIARY_SEND_FILTER, static_cast<ALint>(effect_slot), 0, AL_FILTER_NULL);

		if (alGetError() != AL_NO_ERROR)
		{
			log::warn("Error attaching distortion effect to source!");
		}
		else
		{
			s_data.effect_datas[source->m_source_handle] = {
				.slot = effect_slot,
				.effect = effect,
				.type = audio_effect::distortion
			};
		}
	}

	void audio_effect_manager::apply_flanger(const std::shared_ptr<audio_source>& source, float rate, float depth, float feedback)
	{
		if (!source)
		{
			log::error("Unloaded audio source passed to audio effect manager!");
			return;
		}

		ALuint effect;
		alGenEffects(1, &effect);
		alEffecti(effect, AL_EFFECT_TYPE, AL_EFFECT_FLANGER);
		alEffectf(effect, AL_FLANGER_RATE, rate);
		alEffectf(effect, AL_FLANGER_DEPTH, depth);
		alEffectf(effect, AL_FLANGER_FEEDBACK, feedback);

		ALuint effect_slot;
		alGenAuxiliaryEffectSlots(1, &effect_slot);
		alAuxiliaryEffectSloti(effect_slot, AL_EFFECTSLOT_EFFECT, static_cast<ALint>(effect));
		alSource3i(source->m_source_handle, AL_AUXILIARY_SEND_FILTER, static_cast<ALint>(effect_slot), 0, AL_FILTER_NULL);

		if (alGetError() != AL_NO_ERROR)
		{
			log::warn("Error attaching flanger effect to source!");
		}
		else
		{
			s_data.effect_datas[source->m_source_handle] = {
				.slot = effect_slot,
				.effect = effect,
				.type = audio_effect::flanger
			};
		}
	}

	void audio_effect_manager::apply_equalizer(const std::shared_ptr<audio_source>& source, float low_gain, float mid_gain, float high_gain)
	{
		if (!source)
		{
			log::error("Unloaded audio source passed to audio effect manager!");
			return;
		}

		ALuint effect;
		alGenEffects(1, &effect);
		alEffecti(effect, AL_EFFECT_TYPE, AL_EFFECT_EQUALIZER);
		alEffectf(effect, AL_EQUALIZER_LOW_GAIN, low_gain);
		alEffectf(effect, AL_EQUALIZER_MID1_GAIN, mid_gain);
		alEffectf(effect, AL_EQUALIZER_HIGH_GAIN, high_gain);

		ALuint effect_slot;
		alGenAuxiliaryEffectSlots(1, &effect_slot);
		alAuxiliaryEffectSloti(effect_slot, AL_EFFECTSLOT_EFFECT, static_cast<ALint>(effect));
		alSource3i(source->m_source_handle, AL_AUXILIARY_SEND_FILTER, static_cast<ALint>(effect_slot), 0, AL_FILTER_NULL);

		if (alGetError() != AL_NO_ERROR)
		{
			log::warn("Error attaching equalizer effect to source!");
		}
		else
		{
			s_data.effect_datas[source->m_source_handle] = {
				.slot = effect_slot,
				.effect = effect,
				.type = audio_effect::equalizer
			};
		}
	}

	void audio_effect_manager::apply_frequency_shifter(const std::shared_ptr<audio_source>& source, float frequency, int direction)
	{
		if (!source)
		{
			log::error("Unloaded audio source passed to audio effect manager!");
			return;
		}

		ALuint effect;
		alGenEffects(1, &effect);
		alEffecti(effect, AL_EFFECT_TYPE, AL_EFFECT_FREQUENCY_SHIFTER);
		alEffectf(effect, AL_FREQUENCY_SHIFTER_FREQUENCY, frequency);
		alEffecti(effect, AL_FREQUENCY_SHIFTER_LEFT_DIRECTION, direction);
		alEffecti(effect, AL_FREQUENCY_SHIFTER_RIGHT_DIRECTION, direction);

		ALuint effect_slot;
		alGenAuxiliaryEffectSlots(1, &effect_slot);
		alAuxiliaryEffectSloti(effect_slot, AL_EFFECTSLOT_EFFECT, static_cast<ALint>(effect));
		alSource3i(source->m_source_handle, AL_AUXILIARY_SEND_FILTER, static_cast<ALint>(effect_slot), 0, AL_FILTER_NULL);

		if (alGetError() != AL_NO_ERROR)
		{
			log::warn("Error attaching frequency shifter effect to source!");
		}
		else
		{
			s_data.effect_datas[source->m_source_handle] = {
				.slot = effect_slot,
				.effect = effect,
				.type = audio_effect::frequency_shifter
			};
		}
	}

	void audio_effect_manager::apply_autowah(const std::shared_ptr<audio_source>& source, float attack_time, float release_time, float resonance)
	{
		if (!source)
		{
			log::error("Unloaded audio source passed to audio effect manager!");
			return;
		}

		ALuint effect;
		alGenEffects(1, &effect);
		alEffecti(effect, AL_EFFECT_TYPE, AL_EFFECT_AUTOWAH);
		alEffectf(effect, AL_AUTOWAH_ATTACK_TIME, attack_time);
		alEffectf(effect, AL_AUTOWAH_RELEASE_TIME, release_time);
		alEffectf(effect, AL_AUTOWAH_RESONANCE, resonance);

		ALuint effect_slot;
		alGenAuxiliaryEffectSlots(1, &effect_slot);
		alAuxiliaryEffectSloti(effect_slot, AL_EFFECTSLOT_EFFECT, static_cast<ALint>(effect));
		alSource3i(source->m_source_handle, AL_AUXILIARY_SEND_FILTER, static_cast<ALint>(effect_slot), 0, AL_FILTER_NULL);

		if (alGetError() != AL_NO_ERROR)
		{
			log::warn("Error attaching autowah effect to source!");
		}
		else
		{
			s_data.effect_datas[source->m_source_handle] = {
				.slot = effect_slot,
				.effect = effect,
				.type = audio_effect::autowah
			};
		}
	}

	void audio_effect_manager::apply_ring_modulator(const std::shared_ptr<audio_source>& source, float frequency, float highpass_cutoff)
	{
		if (!source)
		{
			log::error("Unloaded audio source passed to audio effect manager!");
			return;
		}

		ALuint effect;
		alGenEffects(1, &effect);
		alEffecti(effect, AL_EFFECT_TYPE, AL_EFFECT_RING_MODULATOR);
		alEffectf(effect, AL_RING_MODULATOR_FREQUENCY, frequency);
		alEffectf(effect, AL_RING_MODULATOR_HIGHPASS_CUTOFF, highpass_cutoff);

		ALuint effect_slot;
		alGenAuxiliaryEffectSlots(1, &effect_slot);
		alAuxiliaryEffectSloti(effect_slot, AL_EFFECTSLOT_EFFECT, static_cast<ALint>(effect));
		alSource3i(source->m_source_handle, AL_AUXILIARY_SEND_FILTER, static_cast<ALint>(effect_slot), 0, AL_FILTER_NULL);

		if (alGetError() != AL_NO_ERROR)
		{
			log::warn("Error attaching ring modulator effect to source!");
		}
		else
		{
			s_data.effect_datas[source->m_source_handle] = {
				.slot = effect_slot,
				.effect = effect,
				.type = audio_effect::ring_modulator
			};
		}
	}

	void audio_effect_manager::remove_effect(const std::shared_ptr<audio_source>& source, audio_effect type)
	{
		if (!source)
		{
			log::error("Unloaded audio source passed to audio effect manager!");
			return;
		}

		auto it = s_data.effect_datas.find(source->m_source_handle);
		if (it != s_data.effect_datas.end())
		{
			if (it->second.type == type)
			{
				ALuint effect_slot = it->second.slot;
				ALuint effect = it->second.effect;

				alSource3i(source->m_source_handle, AL_AUXILIARY_SEND_FILTER, AL_EFFECTSLOT_NULL, 0, AL_FILTER_NULL);
				alDeleteEffects(1, &effect);
				alDeleteAuxiliaryEffectSlots(1, &effect_slot);

				s_data.effect_datas.erase(it);
			}
		}
	}
}

// NOLINTEND(readability-suspicious-call-argument)