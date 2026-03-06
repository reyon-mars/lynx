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

	[[nodiscard]] std::vector<std::string> split(std::string_view str, std::string_view delimeter)
	{
		std::vector<std::string> result;

		if (str.empty())
		{
			return result;
		}

		if (delimeter.empty())
		{
			result.emplace_back(str);
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
			start = end + delimeter.size();
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

	std::string url_decode(std::string_view str)
	{
		std::string decoded;
		decoded.reserve(str.size());

		for (size_t i = 0; i < str.size(); i++)
		{
			const char c = str[i];
			if (c == '%' && (i + 2) < str.size())
			{
				int value{};
				auto [ptr, ec] = std::from_chars(str.data() + i + 1, str.data() + i + 3, value, 16);
				if (ec == std::errc())
				{
					decoded.push_back(static_cast<char>(value));
					i += 2;
					continue;
				}
			}
			if (c == '+')
			{
				decoded.push_back(' ');
			}
			else
			{
				decoded.push_back(c);
			}
		}
		return decoded;
	}

	std::string url_encode(std::string_view str)
	{
		std::string encoded;
		encoded.reserve(str.size() * 2);

		auto is_safe = [](unsigned char c)
		{
			return std::isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~';
		};

		std::string_view hex = "0123456789ABCDEF";

		for (const unsigned char c : str)
		{
			if (is_safe(c))
			{
				encoded.push_back(static_cast<char>(c));
			}
			else
			{
				encoded.push_back('%');
				encoded.push_back(hex[c >> 4]);
				encoded.push_back(hex[c & 0xF]);
			}
		}
		return encoded;
	}

	std::pair<std::string_view, std::string_view> split_once(std::string_view str, std::string_view delimeter)
	{
		if (str.empty() || delimeter.empty())
		{
			return {str, {}};
		}

		size_t pos = str.find(delimeter);
		if (pos != std::string_view::npos)
		{
			return {str.substr(0, pos), str.substr(pos + delimeter.size())};
		}
		return {str, {}};
	}
} // namespace utils