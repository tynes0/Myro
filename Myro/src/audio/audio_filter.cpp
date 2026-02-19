#include "audio_filter.h"

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

#include "detail/openal_backend.h"

namespace myro
{
	struct filter_data
	{
		ALuint filter;
		audio_filter type;
	};

	struct filter_manager_data
	{
		std::unordered_map<ALuint, filter_data> filter_datas;
	};

	static filter_manager_data s_data;

	void audio_filter_manager::apply_low_pass_filter(const std::shared_ptr<audio_source>& source, float gain, float gainHF)
	{
		if (!source)
		{
			log::error("Unloaded audio source passed to audio filter manager!");
			return;
		}

		ALuint filter;
		alGenFilters(1, &filter);
		alFilteri(filter, AL_FILTER_TYPE, AL_FILTER_LOWPASS);
		alFilterf(filter, AL_LOWPASS_GAIN, gain);
		alFilterf(filter, AL_LOWPASS_GAINHF, gainHF);
		alSourcei(source->m_source_handle, AL_DIRECT_FILTER, filter);

		if (alGetError() != AL_NO_ERROR)
			log::warn("Error attaching low-pass filter to source!");
		else
			s_data.filter_datas[source->m_source_handle] = { filter, audio_filter::low_pass_filter };
	}

	void audio_filter_manager::apply_high_pass_filter(const std::shared_ptr<audio_source>& source, float gain, float gainLF)
	{
		if (!source)
		{
			log::error("Unloaded audio source passed to audio filter manager!");
			return;
		}

		ALuint filter;
		alGenFilters(1, &filter);
		alFilteri(filter, AL_FILTER_TYPE, AL_FILTER_HIGHPASS);
		alFilterf(filter, AL_HIGHPASS_GAIN, gain);
		alFilterf(filter, AL_HIGHPASS_GAINLF, gainLF);
		alSourcei(source->m_source_handle, AL_DIRECT_FILTER, filter);

		if (alGetError() != AL_NO_ERROR)
			log::warn("Error attaching high-pass filter to source!");
		else
			s_data.filter_datas[source->m_source_handle] = { filter, audio_filter::high_pass_filter };
	}

	void audio_filter_manager::remove_filter(const std::shared_ptr<audio_source>& source, audio_filter type)
	{
		if (!source)
		{
			log::error("Unloaded audio source passed to audio filter manager!");
			return;
		}

		auto it = s_data.filter_datas.find(source->m_source_handle);
		if (it != s_data.filter_datas.end())
		{
			if (it->second.type == type)
			{
				ALuint filter = it->second.filter;

				alSource3i(source->m_source_handle, AL_AUXILIARY_SEND_FILTER, AL_EFFECTSLOT_NULL, 0, AL_FILTER_NULL);
				alDeleteFilters(1, &filter);

				s_data.filter_datas.erase(it);
			}
		}
	}
}

