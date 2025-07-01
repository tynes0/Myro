#include "dtlog.h"

#include "log.h"

namespace myro
{
	static dtlog::logger s_logger{ "Myro Log", "[%T - %D] %N: %V" };

	void logger::_trace(const std::string& message)
	{
		s_logger.trace(message);
	}

	void logger::_info(const std::string& message)
	{
		s_logger.info(message);
	}

	void logger::_debug(const std::string& message)
	{
		s_logger.debug(message);
	}

	void logger::_warn(const std::string& message)
	{
		s_logger.warning(message);
	}

	void logger::_error(const std::string& message)
	{
		s_logger.error(message);
	}

	void logger::_critical(const std::string& message)
	{
		s_logger.critical(message);
	}

}