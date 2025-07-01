/*
 * This file is part of the dtlog library, originally created by Tynes0.
 * For the latest version and updates, please visit the official dtlog GitHub repository:
 * https://github.com/tynes0/dtlog
 *
 * dtlog is a basic library for logging, providing fast and user-friendly use
 * It is released under the Apache License 2.0. See the LICENSE file in the root of the dtlog repository
 * or visit the above GitHub link for more details.
 *
 * For contributions, bug reports, or other inquiries, feel free to contact the author:
 * - GitHub: https://github.com/tynes0
 * - Email: cihanbilgihan@gmail.com
 */

#include "dtlog.h"

#ifdef _WIN32

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // WIN32_LEAN_AND_MEAN
#include <Windows.h>

void dtlog::logger::set_stdout_color(log_level level)
{
	WORD color_code = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;

	switch (level)
	{
	case log_level::none:
	case log_level::trace:
		break;
	case log_level::info:
		color_code = FOREGROUND_GREEN;
		break;
	case log_level::debug:
		color_code = FOREGROUND_BLUE;
		break;
	case log_level::warning:
		color_code = FOREGROUND_RED | FOREGROUND_GREEN;
		break;
	case log_level::error:
		color_code = FOREGROUND_RED;
		break;
	case log_level::critical:
		color_code = BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE | FOREGROUND_RED;
		break;
	default:
		break;
	}

	HANDLE console_handle = GetStdHandle(STD_OUTPUT_HANDLE);
	if (console_handle == INVALID_HANDLE_VALUE)
		throw std::invalid_argument("INVALID STD HANDLE (logger::set_stdout_color())");
	SetConsoleTextAttribute(console_handle, color_code);
}

void dtlog::logger::set_stderr_color(log_level level)
{
	WORD color_code = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;

	switch (level)
	{
	case log_level::none:
	case log_level::trace:
		break;
	case log_level::info:
		color_code = FOREGROUND_GREEN;
		break;
	case log_level::debug:
		color_code = FOREGROUND_BLUE;
		break;
	case log_level::warning:
		color_code = FOREGROUND_RED | FOREGROUND_GREEN;
		break;
	case log_level::error:
		color_code = FOREGROUND_RED;
		break;
	case log_level::critical:
		color_code = BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE | FOREGROUND_RED;
		break;
	default:
		break;
	}

	HANDLE console_handle = GetStdHandle(STD_ERROR_HANDLE);
	if (console_handle == INVALID_HANDLE_VALUE)
		throw std::invalid_argument("INVALID STD HANDLE (logger::set_stderr_color())");
	SetConsoleTextAttribute(console_handle, color_code);
}
#else // _WIN32


void dtlog::logger::set_stdout_color(log_level level)
{
	const char* color_code = "\x1b[0m";

	switch (level)
	{
	case log_level::none:
	case log_level::trace:
		break;
	case log_level::info:
		color_code = "\x1b[32m";
		break;
	case log_level::debug:
		color_code = "\x1b[34m";
		break;
	case log_level::warning:
		color_code = "\x1b[33m";
		break;
	case log_level::error:
		color_code = "\x1b[31m";
		break;
	case log_level::critical:
		color_code = "\x1b[91m";
		break;
	default:
		break;
	}

	fwrite(color_code, sizeof(char), strlen(color_code), stdout);
}

void dtlog::logger::set_stderr_color(log_level level)
{
	const char* color_code = "\x1b[0m";

	switch (level)
	{
	case log_level::none:
	case log_level::trace:
		break;
	case log_level::info:
		color_code = "\x1b[32m";
		break;
	case log_level::debug:
		color_code = "\x1b[34m";
		break;
	case log_level::warning:
		color_code = "\x1b[33m";
		break;
	case log_level::error:
		color_code = "\x1b[31m";
		break;
	case log_level::critical:
		color_code = "\x1b[91m";
		break;
	default:
		break;
	}

	fwrite(color_code, sizeof(char), strlen(color_code), stderr);
}

#endif // _WIN32