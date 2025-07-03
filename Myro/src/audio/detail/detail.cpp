#include "detail.h"

#include "../../core/log.h"

namespace myro
{
	void myro::detail::display_file_info(const std::string& filename, int64_t channels, int64_t sample_rate, uint64_t size)
	{
		log::info("~~~~ FILE INFO : {} ~~~~", filename);
		log::info("Channels: {}", channels);
		log::info("Sample rate: {}", sample_rate);
		log::info("Expected Size: {}", size);
	}
}