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
			uint8_t active_log_levels;
		}g_logger_context;
	}

	void log::set_logger_activity(uint8_t active_log_levels)
	{
		std::lock_guard<std::mutex> lock(g_logger_context.mut);
		g_logger_context.active_log_levels = active_log_levels;
	}

	bool log::is_logger_active(level_ level)
	{
		std::lock_guard<std::mutex> lock(g_logger_context.mut);
		return g_logger_context.active_log_levels & static_cast<uint8_t>(level);
	}

	void log::_trace(const std::string& message)
	{
		if (!is_logger_active(level_trace)) return;
		g_logger_context.logger.trace(message);
	}

	void log::_info(const std::string& message)
	{
		if (!is_logger_active(level_info)) return;
		g_logger_context.logger.info(message);
	}

	void log::_debug(const std::string& message)
	{
		if (!is_logger_active(level_debug)) return;
		g_logger_context.logger.debug(message);
	}

	void log::_warn(const std::string& message)
	{
		if (!is_logger_active(level_warn)) return;
		g_logger_context.logger.warning(message);
	}

	void log::_error(const std::string& message)
	{
		if (!is_logger_active(level_error)) return;
		g_logger_context.logger.error(message);
	}

	void log::_critical(const std::string& message)
	{
		if (!is_logger_active(level_critical)) return;
		g_logger_context.logger.critical(message);
	}

}