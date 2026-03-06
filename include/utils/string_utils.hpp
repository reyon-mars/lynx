#pragma once
#include <optional>
#include <string>
#include <string_view>
#include <vector>
namespace utils
{
	std::string_view trim(std::string_view str);
	std::string to_lower(std::string_view str);
	std::vector<std::string> split(std::string_view str, char delimeter);
	std::optional<int> to_int(std::string_view str);
	std::string url_decode(std::string_view encoded);
	std::string url_encode(std::string_view decoded);
} // namespace utils