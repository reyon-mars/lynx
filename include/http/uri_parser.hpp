#pragma once
#include <map>
#include <string>
#include <string_view>
#include <vector>
namespace http
{
	struct parsed_uri
	{
		std::string path;
		std::string query;
		std::map<std::string, std::string> params;
	};

	struct match_result
	{
		std::map<std::string, std::string> params;
		bool matched;
	};

	parsed_uri parse_uri(std::string_view uri);

	std::vector<std::string> split_path(std::string_view path);

	match_result match_path(std::string_view pattern, std::string_view path);
} // namespace http