#pragma once
#include <map>
#include <optional>
#include <filesystem>
#include <string>
#include <string_view>
#include <vector>
namespace http
{
	namespace fs = std::filesystem;
	
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

	std::optional<fs::path> safe_resolve( const fs::path& base_dir, const std::string& uri_path );
} // namespace http