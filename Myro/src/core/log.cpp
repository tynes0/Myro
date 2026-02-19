#include "log.h"

#include <dtlog.h>
#include <mutex>

namespace myro
{
	namespace 
	{
		struct
		{
			std::mutex mut;
			dtlog::logger<> logger{ "Myro Log", "[%T - %D] %N: %V\n" };
			bool active = true;
		}g_logger_context;
	}

	void log::set_logger_activity(bool is_active)
	{
		std::lock_guard<std::mutex> lock(g_logger_context.mut);
		g_logger_context.active = is_active;
	}

	bool log::is_logger_active()
	{
		std::lock_guard<std::mutex> lock(g_logger_context.mut);
		return g_logger_context.active;
	}

	void log::_trace(const std::string& message)
	{
		if (!g_logger_context.active) return;
		g_logger_context.logger.trace(message);
	}

	void log::_info(const std::string& message)
	{
		if (!g_logger_context.active) return;
		g_logger_context.logger.info(message);
	}

	void log::_debug(const std::string& message)
	{
		if (!g_logger_context.active) return;
		g_logger_context.logger.debug(message);
	}

	void log::_warn(const std::string& message)
	{
		if (!g_logger_context.active) return;
		g_logger_context.logger.warning(message);
	}

	void log::_error(const std::string& message)
	{
		if (!g_logger_context.active) return;
		g_logger_context.logger.error(message);
	}

	void log::_critical(const std::string& message)
	{
		if (!g_logger_context.active) return;
		g_logger_context.logger.critical(message);
	}

}