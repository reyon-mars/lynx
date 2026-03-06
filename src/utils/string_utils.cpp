#include "utils/string_utils.hpp"
#include <algorithm>
#include <cctype>
#include <charconv>
#include <cstddef>
#include <functional>
#include <iterator>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace utils
{
	std::string_view trim(std::string_view str)
	{
		const auto start = str.find_first_not_of(" \t\r\f\n\v");
		if (start == std::string::npos)
		{
			return {};
		}
		const auto end = str.find_last_not_of(" \t\r\f\n\v");
		return str.substr(start, end - start + 1);
	}

	std::string to_lower(std::string_view str)
	{
		std::string result;
		result.reserve(str.size());
		std::ranges::transform(str,
							   std::back_inserter(result),
							   [](unsigned char c)
							   {
								   return static_cast<char>(std::tolower(c));
							   });
		return result;
	}

	[[nodiscard]] std::vector<std::string> split(std::string_view str, char delimeter)
	{
		std::vector<std::string> result;
		if (str.empty())
		{
			return result;
		}

		size_t start = 0;
		size_t end = str.find(delimeter);

		while (end != std::string_view::npos)
		{
			std::string_view segment = str.substr(start, end - start);
			if (!segment.empty())
			{
				result.emplace_back(segment);
			}
			start = end + 1;
			end = str.find(delimeter, start);
		}
		std::string_view last_segment = str.substr(start, end);
		if (!last_segment.empty())
		{
			result.emplace_back(str.substr(start, end));
		}
		return result;
	}

	bool equals_insensitive(std::string_view a, std::string_view b)
	{
		return std::ranges::equal(a,
								  b,
								  [](unsigned char ac, unsigned char bc)
								  {
									  return std::tolower(ac) == std::tolower(bc);
								  });
	}

	std::optional<int> to_int(std::string_view str)
	{
		int value;
		auto [ptr, ec] = std::from_chars(str.begin(), str.end(), value);
		if (ec == std::errc())
		{
			return value;
		}
		return std::nullopt;
	}
} // namespace utils