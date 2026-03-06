#pragma once
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
namespace utils
{
	std::string_view trim(std::string_view str);
	std::string to_lower(std::string_view str);
	std::vector<std::string> split(std::string_view str, std::string_view delimeter);
	std::pair<std::string_view, std::string_view> split_once(std::string_view str, std::string_view delimeter);
	bool equals_insensitive(std::string_view a, std::string_view b);
	std::optional<int> to_int(std::string_view str);
	std::string url_decode(std::string_view encoded);
	std::string url_encode(std::string_view decoded);
} // namespace utils