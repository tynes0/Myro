#include "dtlog.h"

#include "log.h"

namespace myro
{
	static dtlog::logger s_logger{ "Myro Log", "[%T - %D] %N: %V\n" };

	void log::_trace(const std::string& message)
	{
		s_logger.trace(message);
	}

	void log::_info(const std::string& message)
	{
		s_logger.info(message);
	}

	void log::_debug(const std::string& message)
	{
		s_logger.debug(message);
	}

	void log::_warn(const std::string& message)
	{
		s_logger.warning(message);
	}

	void log::_error(const std::string& message)
	{
		s_logger.error(message);
	}

	void log::_critical(const std::string& message)
	{
		s_logger.critical(message);
	}

}