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

#pragma once

 // @brief Disables the secure warnings for unsafe functions on MSVC compiler.
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif // _CRT_SECURE_NO_WARNINGS

#include <string>    // @brief Include for std::string.
#include <sstream>   // @brief Include for std::ostringstream.
#include <iomanip>   // @brief Include for std::setw and std::setfill.

#if _HAS_NODISCARD
#define DTLOG_NODISCARD [[nodiscard]]  // @brief If _HAS_NODISCARD is defined, DTLOG_NODISCARD expands to [[nodiscard]].
#else // _HAS_NODISCARD
#define DTLOG_NODISCARD  // @brief Otherwise, it expands to nothing.
#endif // _HAS_NODISCARD

namespace dtlog
{
    /**
     * @brief A helper template class for managing a dynamic array of elements.
     *
     * This class provides basic functionalities like adding, accessing, and managing
     * elements in a dynamic array. It includes methods for size, capacity, and
     * iterators for the beginning and end of the array.
     */
    template <typename T>
    class helper_vector
    {
    public:
        /**
         * @brief Constructor initializes the vector with null data, zero capacity, and size.
         */
        helper_vector() : m_data(nullptr), m_capacity(0), m_size(0) {}

        /**
         * @brief Destructor cleans up the dynamically allocated data.
         */
        ~helper_vector()
        {
            delete[] m_data;
        }

        /**
         * @brief Overloaded subscript operator for accessing elements by index with bounds checking.
         * @param index The index of the element to access.
         * @return Reference to the element at the specified index.
         * @throws std::out_of_range if index is out of bounds.
         */
        T& operator[](size_t index)
        {
            if (index >= m_size)
                throw std::out_of_range("Index out of range!");
            return m_data[index];
        }

        /**
         * @brief Const version of the overloaded subscript operator for accessing elements by index with bounds checking.
         * @param index The index of the element to access.
         * @return Const reference to the element at the specified index.
         * @throws std::out_of_range if index is out of bounds.
         */
        const T& operator[](size_t index) const
        {
            if (index >= m_size)
                throw std::out_of_range("Index out of range!");
            return m_data[index];
        }

        /**
         * @brief Adds a new element to the end of the vector.
         * @param value The value to add.
         */
        void push_back(const T& value)
        {
            if (m_size >= m_capacity)
            {
                size_t new_capacity = (m_capacity == 0) ? 1 : m_capacity * 2;
                reserve(new_capacity);
            }
            m_data[m_size++] = value;
        }

        /**
         * @brief Removes the last element of the vector.
         */
        void pop_back()
        {
            if (m_size > 0)
                --m_size;
        }

        /**
         * @brief Gets the current size of the vector.
         * @return The number of elements in the vector.
         */
        size_t size() const
        {
            return m_size;
        }

        /**
         * @brief Gets the current capacity of the vector.
         * @return The capacity of the vector.
         */
        size_t capacity() const
        {
            return m_capacity;
        }

        /**
         * @brief Returns a pointer to the beginning of the vector.
         * @return Pointer to the first element of the vector.
         */
        T* begin() const
        {
            return m_data;
        }

        /**
         * @brief Returns a pointer to the end of the vector.
         * @return Pointer to one past the last element of the vector.
         */
        T* end() const
        {
            return m_data + m_size;
        }

    private:
        /**
         * @brief Reserves memory for the vector.
         * @param new_capacity The new capacity for the vector.
         */
        void reserve(size_t new_capacity)
        {
            T* new_data = new T[new_capacity];
            std::memcpy(new_data, m_data, sizeof(T) * m_size);
            delete[] m_data;
            m_data = new_data;
            m_capacity = new_capacity;
        }
    private:
        T* m_data;         ///< Pointer to the dynamically allocated array.
        size_t m_capacity; ///< The current capacity of the vector.
        size_t m_size;     ///< The current size of the vector.
    };

    /**
     * @brief A utility class for formatting strings.
     */
    class formatter
    {
    public:
        /**
         * @brief Formats a string with the given arguments.
         * @tparam _Args The types of the arguments.
         * @param fmt The format string.
         * @param args The arguments to format into the string.
         * @return The formatted string.
         */
        template <typename... _Args>
        DTLOG_NODISCARD static std::string format(const std::string& fmt, _Args&&... args)
        {
            if (sizeof...(args) == 0)
            {
                return fmt;
            }

            argument_array argArray;
            transfer_to_array(argArray, std::forward<_Args>(args)...);
            size_t start = 0;
            size_t pos = 0;
            std::ostringstream oss;
            while (true)
            {
                pos = fmt.find('{', start);
                if (pos == std::string::npos)
                {
                    oss << fmt.substr(start);
                    break;
                }

                oss << fmt.substr(start, pos - start);
                if (fmt[pos + 1] == '{')
                {
                    oss << '{';
                    start = pos + 2;
                    continue;
                }

                start = pos + 1;
                pos = fmt.find('}', start);
                if (pos == std::string::npos)
                {
                    oss << fmt.substr(start - 1);
                    break;
                }

                format_item(oss, fmt.substr(start, pos - start), argArray);
                start = pos + 1;
            }

            return oss.str();
        }

        /**
         * @brief Overloaded function call operator to format strings.
         * @tparam Args The types of the arguments.
         * @param fmt The format string.
         * @param args The arguments to format into the string.
         * @return The formatted string.
         */
        template <typename... Args>
        std::string operator()(const std::string& fmt, Args&&... args)
        {
            return format(fmt, std::forward<Args>(args)...);
        }

    private:
        /**
         * @brief Base class for arguments used in formatting.
         */
        struct argument_base
        {
            argument_base() {}
            virtual ~argument_base() {}
            virtual void format(std::ostringstream&) {}
        };

        /**
         * @brief Template class for holding arguments of various types.
         * @tparam _Ty The type of the argument.
         */
        template <class _Ty>
        class argument : public argument_base
        {
        public:
            /**
             * @brief Constructs an argument with the given value.
             * @param arg The value of the argument.
             */
            argument(_Ty arg) : m_argument(arg) {}

            /**
             * @brief Destructor.
             */
            virtual ~argument() override {}

            /**
             * @brief Formats the argument into the output stream.
             * @param oss The output stream.
             */
            virtual void format(std::ostringstream& oss) override
            {
                oss << m_argument;
            }

        private:
            _Ty m_argument; ///< The value of the argument.
        };

        /**
         * @brief A helper vector for managing an array of argument_base pointers.
         */
        class argument_array : public helper_vector<argument_base*>
        {
        public:
            argument_array() {}
            ~argument_array()
            {
                for (auto item : *this)
                {
                    delete item;
                }
            }
        };

        /**
         * @brief Formats a single item into the output stream.
         * @param oss The output stream.
         * @param item The item to format.
         * @param arguments The array of arguments.
         */
        static void format_item(std::ostringstream& oss, const std::string& item, const argument_array& arguments)
        {
            size_t index = 0;
            char* endptr = nullptr;
#if _WIN64
            index = std::strtoull(&item[0], &endptr, 10);
#else // !_WIN64
            index = std::strtoul(&item[0], &endptr, 10);
#endif // _WIN64

            if (index >= arguments.size())
                return;
            arguments[index]->format(oss);
        }

        /**
         * @brief Transfers the arguments into the argument array.
         * @tparam _Arg The type of the first argument.
         * @tparam _Ty The types of the remaining arguments.
         * @param arguments The argument array.
         * @param first The first argument.
         * @param rest The remaining arguments.
         */
        template <typename _Arg, typename... _Ty>
        static void transfer_to_array(argument_array& arguments, _Arg&& first, _Ty&&... rest)
        {
            arguments.push_back(new argument<_Arg>(std::forward<_Arg>(first)));
            transfer_to_array(arguments, std::forward<_Ty>(rest)...);
        }

        /**
         * @brief Base case for transferring arguments into the argument array.
         * @param arguments The argument array.
         */
        static void transfer_to_array(argument_array& arguments) {}
    };

    /**
 * @brief A utility class for formatting date and time strings.
 */
    class date_time_formatter
    {
    public:
        /**
         * @brief Default constructor.
         * Initializes the formatter with the current local time.
         */
        date_time_formatter()
        {
            reset_time();
        }

        /**
         * @brief Constructor that initializes the formatter with the specified time.
         * @param timeptr Pointer to a std::tm structure representing the time.
         */
        explicit date_time_formatter(const std::tm* timeptr) : m_timeptr(timeptr) {}

#pragma warning(push)
#pragma warning(disable : 4996)
        /**
         * @brief Resets the time to the current local time.
         */
        void reset_time()
        {
            std::time_t t = std::time(nullptr);
            m_timeptr = std::localtime(&t);
        }
#pragma warning(pop)

        /**
         * @brief Gets the full name of the weekday.
         * @return The full name of the weekday.
         */
        DTLOG_NODISCARD std::string full_weekday_name() const
        {
            return weekdays(m_timeptr->tm_wday);
        }

        /**
         * @brief Gets the full name of the month.
         * @return The full name of the month.
         */
        DTLOG_NODISCARD std::string full_month_name() const
        {
            return months(m_timeptr->tm_mon);
        }

        /**
         * @brief Gets the last two digits of the year.
         * @return The last two digits of the year.
         */
        DTLOG_NODISCARD std::string year_2_digits() const
        {
            return std::to_string(m_timeptr->tm_year % 100);
        }

        /**
         * @brief Gets the full four digits of the year.
         * @return The full four digits of the year.
         */
        DTLOG_NODISCARD std::string year_4_digits() const
        {
            return std::to_string(m_timeptr->tm_year + 1900);
        }

        /**
         * @brief Gets the date and time representation in a specific format.
         * @return The formatted date and time string.
         */
        DTLOG_NODISCARD std::string date_time_representation() const
        {
            std::ostringstream oss;
            oss << weekdays(m_timeptr->tm_wday)
                << " "
                << months(m_timeptr->tm_mon)
                << " "
                << m_timeptr->tm_mday
                << " "
                << m_timeptr->tm_year + 1900
                << " "
                << format_time(m_timeptr->tm_hour)
                << ":"
                << format_time(m_timeptr->tm_min)
                << ":"
                << format_time(m_timeptr->tm_sec);
            return oss.str();
        }

        /**
         * @brief Gets the short date representation in MM/DD/YY format.
         * @return The short date string.
         */
        DTLOG_NODISCARD std::string short_MMDDYY_date() const
        {
            std::ostringstream oss;
            oss << format_time(m_timeptr->tm_mon + 1)
                << "/"
                << format_time(m_timeptr->tm_mday)
                << "/"
                << format_time((m_timeptr->tm_year % 100));
            return oss.str();
        }

        /**
         * @brief Gets the month as a numeric string.
         * @return The numeric representation of the month.
         */
        DTLOG_NODISCARD std::string month() const
        {
            return std::to_string(m_timeptr->tm_mon + 1);
        }

        /**
         * @brief Gets the day of the month as a string.
         * @return The day of the month.
         */
        DTLOG_NODISCARD std::string day_of_month() const
        {
            return std::to_string(m_timeptr->tm_mday);
        }

        /**
         * @brief Gets the hours in 24-hour format.
         * @return The hours in 24-hour format.
         */
        DTLOG_NODISCARD std::string hours_24_format() const
        {
            return std::to_string(m_timeptr->tm_hour);
        }

        /**
         * @brief Gets the hours in 12-hour format.
         * @return The hours in 12-hour format.
         */
        DTLOG_NODISCARD std::string hours_12_format() const
        {
            int hours12 = m_timeptr->tm_hour % 12;
            if (hours12 == 0)
                hours12 = 12;
            return std::to_string(hours12);
        }

        /**
         * @brief Gets the minutes as a string.
         * @return The minutes.
         */
        DTLOG_NODISCARD std::string minutes() const
        {
            return std::to_string(m_timeptr->tm_min);
        }

        /**
         * @brief Gets the seconds as a
         * string.
         * @return The seconds.
         */
        DTLOG_NODISCARD std::string seconds() const
        {
            return std::to_string(m_timeptr->tm_sec);
        }

        /**
         * @brief Gets the AM/PM designation.
         * @return "AM" if the time is before noon, "PM" otherwise.
         */
        DTLOG_NODISCARD std::string AM_PM() const
        {
            return (m_timeptr->tm_hour < 12) ? "AM" : "PM";
        }

        /**
         * @brief Gets the time in 12-hour clock format.
         * @return The time in 12-hour format along with AM/PM designation.
         */
        DTLOG_NODISCARD std::string clock_12_hour() const
        {
            std::ostringstream oss;
            oss << format_time((m_timeptr->tm_hour % 12 == 0) ? 12 : m_timeptr->tm_hour % 12)
                << ":"
                << format_time(m_timeptr->tm_min)
                << ":"
                << format_time(m_timeptr->tm_sec) << " "
                << ((m_timeptr->tm_hour < 12) ? "AM" : "PM");
            return oss.str();
        }

        /**
         * @brief Gets the time in HH:MM format (24-hour clock).
         * @return The time in HH:MM format.
         */
        DTLOG_NODISCARD std::string HHMM_time_24_hour() const
        {
            std::ostringstream oss;
            oss << format_time(m_timeptr->tm_hour)
                << ":"
                << format_time(m_timeptr->tm_min);
            return oss.str();
        }

        /**
         * @brief Gets the time in ISO 8601 format (HH:MM:SS).
         * @return The time in ISO 8601 format.
         */
        DTLOG_NODISCARD std::string ISO8601_time_format() const
        {
            std::ostringstream oss;
            oss << format_time(m_timeptr->tm_hour)
                << ":"
                << format_time(m_timeptr->tm_min)
                << ":"
                << format_time(m_timeptr->tm_sec);
            return oss.str();
        }

    private:
        /**
         * @brief Formats the given time value with leading zeros if necessary.
         * @param time_value The time value to format.
         * @return The formatted time string.
         */
        DTLOG_NODISCARD std::string format_time(int time_value) const
        {
            std::ostringstream oss;
            oss << std::setw(2)
                << std::setfill('0')
                << time_value;
            return oss.str();
        }

        /**
         * @brief Gets the full name of the weekday based on the day of the week.
         * @param wday The day of the week (0-6, Sunday-Saturday).
         * @return The full name of the weekday.
         */
        DTLOG_NODISCARD std::string weekdays(int wday) const
        {
            switch (wday)
            {
            case 0: return "Sunday";
            case 1: return "Monday";
            case 2: return "Tuesday";
            case 3: return "Wednesday";
            case 4: return "Thursday";
            case 5: return "Friday";
            case 6: return "Saturday";
            default: return "Invalid Day";
            }
        }

        /**
         * @brief Gets the full name of the month based on the month index.
         * @param mon The month index (0-11, January-December).
         * @return The full name of the month.
         */
        DTLOG_NODISCARD std::string months(int mon) const
        {
            switch (mon)
            {
            case 0: return "January";
            case 1: return "February";
            case 2: return "March";
            case 3: return "April";
            case 4: return "May";
            case 5: return "June";
            case 6: return "July";
            case 7: return "August";
            case 8: return "September";
            case 9: return "October";
            case 10: return "November";
            case 11: return "December";
            default: return "Invalid Month";
            }
        }

    private:
        const std::tm* m_timeptr;
    };

    /**
     * @brief Enumeration for different log levels.
     */
    enum class log_level
    {
        none,
        trace,
        info,
        debug,
        warning,
        error,
        critical
    };

    /**
     * @brief Converts a log level enum to its corresponding string representation.
     * @param level The log level enum.
     * @return The string representation of the log level.
     */
    DTLOG_NODISCARD inline std::string log_level_to_string(log_level level)
    {
        switch (level)
        {
        case dtlog::log_level::trace:
            return "trace";
        case dtlog::log_level::info:
            return "info";
        case dtlog::log_level::debug:
            return "debug";
        case dtlog::log_level::warning:
            return "warning";
        case dtlog::log_level::error:
            return "error";
        case dtlog::log_level::critical:
            return "critical";
        case dtlog::log_level::none:
        default:
            return "none";
        }
    }

    /**
     * @brief A class for logging messages with various log levels and formatting options.
     */
    class logger
    {
    public:
        /**
         * @brief Constructor for the logger.
         * @param log_name The name of the logger.
         * @param pattern The log message pattern.
         */
        logger(const std::string& log_name = "dtlog", const std::string& pattern = "[%R] %N: %V") : log_name(log_name), log_pattern(pattern) {}

        /**
         * @brief Logs a message with the specified log level.
         * @tparam _Args Variadic template for message arguments.
         * @param level The log level.
         * @param message The log message.
         * @param args Additional arguments for formatting the message.
         */
        template <class ..._Args>
        void log(log_level level, const std::string& message, _Args&&... args)
        {
            std::string formatted_message = formatter::format(message, std::forward<_Args>(args)...);
            std::string log_message;
            pattern(level, formatted_message, log_message);
            set_stdout_color(level);
            std::fwrite(log_message.c_str(), sizeof(char), log_message.length(), stdout);
            std::fflush(stdout);
            set_stdout_color(log_level::none);
        }

        /**
         * @brief Logs a message with the specified log level to stderr.
         * @tparam _Args Variadic template for message arguments.
         * @param level The log level.
         * @param message The log message.
         * @param args Additional arguments for formatting the message.
         */
        template <class ..._Args>
        void log_stderr(log_level level, const std::string& message, _Args&&... args)
        {
            std::string formatted_message = formatter::format(message, std::forward<_Args>(args)...);
            std::string log_message;
            pattern(level, formatted_message, log_message);
            set_stderr_color(level);
            std::fwrite(log_message.c_str(), sizeof(char), log_message.length(), stderr);
            std::fflush(stderr);
            set_stderr_color(log_level::none);
        }

        /**
         * @brief Logs a message with the specified log level to a file.
         * @tparam _Args Variadic template for message arguments.
         * @param filename The name of the log file.
         * @param message The log message.
         * @param args Additional arguments for formatting the message.
         */
        template <class ..._Args>
        void log_to_file(const std::string& filename, const std::string& message, _Args&&... args)
        {
            FILE* file = std::fopen(filename.c_str(), "a+");
            if (!file)
                return; // It was not successful, but instead of assertion, we just return. We don't simply log to file.
            log_to_file(file, message, std::forward<_Args>(args)...);
            std::fclose(file);
        }

        /**
         * @brief Logs a message with the specified log level to the given file stream.
         * @tparam _Args Variadic template for message arguments.
         * @param file The file stream to log to.
         * @param message The log message.
         * @param args Additional arguments for formatting the message.
         */
        template <class ..._Args>
        void log_to_file(FILE* file, const std::string& message, _Args&&... args)
        {
            if (!file)
                return; // It was not successful, but instead of assertion, we just return. We don't simply log to file.
            std::string formatted_message = formatter::format(message, std::forward<_Args>(args)...);
            std::fwrite(formatted_message.c_str(), sizeof(char), formatted_message.length(), file);
            std::fflush(file);
        }

        /**
         * @brief Sets the name of the logger.
         * @param name The new name for the logger.
         */
        void set_name(const std::string& name)
        {
            log_name = name;
        }

        /**
         * @brief Gets the name of the logger.
         * @return The name of the logger.
         */
        DTLOG_NODISCARD std::string get_name() const
        {
            return log_name;
        }

        /**
         * @brief Sets the log message pattern.
         * @param format The new log message pattern.
         */
        void set_pattern(const std::string& format)
        {
            log_pattern = format;
        }

        /**
         * @brief Gets the log message pattern.
         * @return The log message pattern.
         */
        DTLOG_NODISCARD std::string get_pattern() const
        {
            return log_pattern;
        }

        /**
        * @brief Logs a trace message to the given file stream.
        * @tparam _Args Variadic template for message arguments.
        * @param message The log message.
        * @param args Additional arguments for formatting the message.
        */
        template <class ..._Args>
        void trace(const std::string& message, _Args&&... args)
        {
            return this->log(log_level::trace, message, std::forward<_Args>(args)...);
        }

        /**
        * @brief Logs a info message to the given file stream.
        * @tparam _Args Variadic template for message arguments.
        * @param message The log message.
        * @param args Additional arguments for formatting the message.
        */
        template <class ..._Args>
        void info(const std::string& message, _Args&&... args)
        {
            return this->log(log_level::info, message, std::forward<_Args>(args)...);
        }

        /**
        * @brief Logs a debug message to the given file stream.
        * @tparam _Args Variadic template for message arguments.
        * @param message The log message.
        * @param args Additional arguments for formatting the message.
        */
        template <class ..._Args>
        void debug(const std::string& message, _Args&&... args)
        {
            return this->log(log_level::debug, message, std::forward<_Args>(args)...);
        }

        /**
        * @brief Logs a warning message to the given file stream.
        * @tparam _Args Variadic template for message arguments.
        * @param message The log message.
        * @param args Additional arguments for formatting the message.
        */
        template <class ..._Args>
        void warning(const std::string& message, _Args&&... args)
        {
            return this->log(log_level::warning, message, std::forward<_Args>(args)...);
        }

        /**
        * @brief Logs a error message to the given file stream.
        * @tparam _Args Variadic template for message arguments.
        * @param message The log message.
        * @param args Additional arguments for formatting the message.
        */
        template <class ..._Args>
        void error(const std::string& message, _Args&&... args)
        {
            return this->log(log_level::error, message, std::forward<_Args>(args)...);
        }

        /**
        * @brief Logs a critical message to the given file stream.
        * @tparam _Args Variadic template for message arguments.
        * @param message The log message.
        * @param args Additional arguments for formatting the message.
        */
        template <class ..._Args>
        void critical(const std::string& message, _Args&&... args)
        {
            return this->log(log_level::critical, message, std::forward<_Args>(args)...);
        }

    private:
        /**
         * @brief Formats the log message based on the log pattern.
         * @param level The log level.
         * @param message The log message.
         * @param formatted_message The formatted log message.
         */
        void pattern(log_level level, const std::string& message, std::string& formatted_message)
        {
            date_time_formatter time_formatter;
            formatted_message = log_pattern;
            size_t pos = 0;

            while (true)
            {
                pos = formatted_message.find('%', pos);
                if (pos == std::string::npos || pos == formatted_message.size() - 1)
                    break;
                char token = formatted_message[pos + 1];
                switch (token)
                {
                case 'V':
                    formatted_message.replace(pos, 2, message);
                    break;
                case 'N':
                    formatted_message.replace(pos, 2, log_name);
                    break;
                case 'L':
                    formatted_message.replace(pos, 2, log_level_to_string(level));
                    break;
                case 'A':
                    formatted_message.replace(pos, 2, time_formatter.full_weekday_name());
                    break;
                case 'B':
                    formatted_message.replace(pos, 2, time_formatter.full_month_name());
                    break;
                case 'C':
                    formatted_message.replace(pos, 2, time_formatter.year_2_digits());
                    break;
                case 'Y':
                    formatted_message.replace(pos, 2, time_formatter.year_4_digits());
                    break;
                case 'R':
                    formatted_message.replace(pos, 2, time_formatter.date_time_representation());
                    break;
                case 'D':
                    formatted_message.replace(pos, 2, time_formatter.short_MMDDYY_date());
                    break;
                case 'm':
                    formatted_message.replace(pos, 2, time_formatter.month());
                    break;
                case 'd':
                    formatted_message.replace(pos, 2, time_formatter.day_of_month());
                    break;
                case 'H':
                    formatted_message.replace(pos, 2, time_formatter.hours_24_format());
                    break;
                case 'h':
                    formatted_message.replace(pos, 2, time_formatter.hours_12_format());
                    break;
                case 'M':
                    formatted_message.replace(pos, 2, time_formatter.minutes());
                    break;
                case 'S':
                    formatted_message.replace(pos, 2, time_formatter.seconds());
                    break;
                case 'F':
                    formatted_message.replace(pos, 2, time_formatter.AM_PM());
                    break;
                case 'x':
                    formatted_message.replace(pos, 2, time_formatter.clock_12_hour());
                    break;
                case 'X':
                    formatted_message.replace(pos, 2, time_formatter.HHMM_time_24_hour());
                    break;
                case 'T':
                    formatted_message.replace(pos, 2, time_formatter.ISO8601_time_format());
                    break;
                case '%':
                    formatted_message.replace(pos, 2, "%");
                    break;
                case 'n':
                    formatted_message.replace(pos, 2, "\n");
                    break;
                default:
                    break;
                }
            }
        }

        /**
         * @brief Sets the color for standard output based on the log level.
         * @param level The log level.
         */
        void set_stdout_color(log_level level);

        /**
         * @brief Sets the color for standard error output based on the log level.
         * @param level The log level.
         */
        void set_stderr_color(log_level level);

    private:
        std::string log_name;       // The name of the logger
        std::string log_pattern;    // The log message pattern
    };
} // namespace dtlog