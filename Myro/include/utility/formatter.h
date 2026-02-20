#pragma once

#include <algorithm>
#include <string>
#include <sstream>
#include <utility>
#include <vector>
#include <iomanip>

namespace myro
{
	class formatter
	{
	public:
		template <typename... Args>
		[[nodiscard]] static std::string format(const std::string& fmt, Args&&... args)
		{
			if constexpr (sizeof...(Args) == 0)
			{
				return fmt;
			}

			argument_array argArray;
			transfer_to_array(argArray, std::forward<Args>(args)...);
			size_t start = 0;
			std::ostringstream oss;
			while (true)
			{
				size_t pos = fmt.find('{', start);
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

		template <typename... Args>
		std::string operator()(const std::string& fmt, Args&&... args)
		{
			return format(fmt, std::forward<Args>(args)...);
		}

	private:
		// NOLINTNEXTLINE(cppcoreguidelines-special-member-functions)
		struct argument_base
		{
			argument_base() = default;
			virtual ~argument_base() = default;
			virtual void format(std::ostringstream&, const std::string&) {}
		};
		
		template <class T>
		// NOLINTNEXTLINE(cppcoreguidelines-special-member-functions)
		class argument : public argument_base
		{
		public:
			argument(T arg) : m_argument(std::move(arg)) {}

			~argument() override = default;

			void format(std::ostringstream& oss, const std::string& fmt) override
			{
				oss << m_argument;
			}

		private:
			T m_argument;
		};

		// NOLINTNEXTLINE(cppcoreguidelines-special-member-functions)
		class argument_array : public std::vector<argument_base*>
		{
		public:
			argument_array() = default;
			~argument_array()
			{
				std::ranges::for_each(begin(), end(), [](const argument_base* arg_ptr)->void { delete arg_ptr; });
			}
		};

		static void format_item(std::ostringstream& oss, const std::string& item, const argument_array& arguments)
		{
			size_t index = 0;
			std::string fmt;
			char* endptr = nullptr;
#if _WIN64
			index = strtoull(item.data(), &endptr, 10);
#else // !_WIN64
			index = strtoul(&item[0], &endptr, 10);
#endif // _WIN64

			if (index < 0 || index >= arguments.size())
				return;

			if (*endptr == ',')
			{
				long long alignment = strtoll(endptr + 1, &endptr, 10);
				if (alignment > 0)
					oss << std::right << std::setw(alignment);
				else if (alignment < 0)
					oss << std::left << std::setw(-alignment);
			}

			if (*endptr == ':')
				fmt = endptr + 1;

			arguments[index]->format(oss, fmt);
		}

		static void transfer_to_array(argument_array& arg_array) { (void)arg_array; }

		template <class T>
		static void transfer_to_array(argument_array& arg_array, T arg)
		{
			arg_array.push_back(new argument<T>(std::move(arg)));
		}

		template <class T, class... Args>
		static void transfer_to_array(argument_array& arg_array, T arg, Args&&... args)
		{
			transfer_to_array(arg_array, std::move(arg));
			transfer_to_array(arg_array, std::forward<Args>(args)...);
		}
	};
}
