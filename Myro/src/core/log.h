#pragma once

#include <string>
#include <filesystem>

#include "../util/formatter.h"
#include "base.h"

namespace myro
{
	class log
	{
	public:
		template <class... Args>
		static void trace(const std::string& message, Args... args)
		{
			std::string formatted_msg = formatter::format(message, std::forward<Args>(args)...);
			_trace(formatted_msg);
		}

		template <class... Args>
		static void info(const std::string& message, Args... args)
		{
			std::string formatted_msg = formatter::format(message, std::forward<Args>(args)...);
			_info(formatted_msg);
		}

		template <class... Args>
		static void debug(const std::string& message, Args... args)
		{
			std::string formatted_msg = formatter::format(message, std::forward<Args>(args)...);
			_debug(formatted_msg);
		}

		template <class... Args>
		static void warn(const std::string& message, Args... args)
		{
			std::string formatted_msg = formatter::format(message, std::forward<Args>(args)...);
			_warn(formatted_msg);
		}

		template <class... Args>
		static void error(const std::string& message, Args... args)
		{
			std::string formatted_msg = formatter::format(message, std::forward<Args>(args)...);
			_error(formatted_msg);
		}

		template <class... Args>
		static void critical(const std::string& message, Args... args)
		{
			std::string formatted_msg = formatter::format(message, std::forward<Args>(args)...);
			_critical(formatted_msg);
		}
	private:
		static void _trace(const std::string& message);
		static void _info(const std::string& message);
		static void _debug(const std::string& message);
		static void _warn(const std::string& message);
		static void _error(const std::string& message);
		static void _critical(const std::string& message);
	};

#ifdef MYRO_ENABLE_ASSERTS
	#define _MYRO_INTERNAL_ASSERT_IMPL(check, msg, ...) do { if(!(check)) {myro::log::critical(msg, __VA_ARGS__); MYRO_DEBUGBREAK(); } } while(false)
	#define _MYRO_INTERNAL_ASSERT_WITH_MSG(check, ...)	_MYRO_INTERNAL_ASSERT_IMPL(check, "Myro Assertion failed! File: {0}, Line: {1}, Message: {2}", std::filesystem::path(__FILE__).filename().string(), __LINE__ , __VA_ARGS__)
	#define _MYRO_INTERNAL_ASSERT_NO_MSG(check)			_MYRO_INTERNAL_ASSERT_IMPL(check, "Myro Assertion '{0}' failed! File: {1}, Line: {2}", MYRO_STRINGIZE(check), std::filesystem::path(__FILE__).filename().string(), __LINE__)
	#define _MYRO_INTERNAL_ASSERT_GET_MACRO_NAME(arg1, arg2, macro, ...) macro
	#define _MYRO_INTERNAL_ASSERT_GET_MACRO(...) MYRO_EXPAND(_MYRO_INTERNAL_ASSERT_GET_MACRO_NAME(__VA_ARGS__, _MYRO_INTERNAL_ASSERT_WITH_MSG, _MYRO_INTERNAL_ASSERT_NO_MSG))
	
	#define MYRO_ASSERT(...) MYRO_EXPAND(_MYRO_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(__VA_ARGS__))
#else //MYRO_ENABLE_ASSERTS
	#define MYRO_ASSERT(x, ...)				// Myro assert not enabled
#endif //MYRO_ENABLE_ASSERTS

#define MYRO_UNIMPLAMENTED() MYRO_ASSERT(false, "ERROR: Unimplamented method called!");
}