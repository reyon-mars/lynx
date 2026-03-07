#include "http/uri_parser.hpp"
#include "utils/string_utils.hpp"
#include <cstddef>
#include <string>
#include <string_view>
#include <tuple>
#include <vector>

namespace http
{
	parsed_uri parse_uri(std::string_view uri)
	{
		parsed_uri result{};

		auto split_result = utils::split_once(uri, "?");
		std::string_view raw_path{uri};
		std::string_view raw_query{};

		if (split_result)
		{
			std::tie(raw_path, raw_query) = split_result.value();
		}
		result.path = utils::url_decode(raw_path);
		result.query = raw_query;

		auto query_pairs = utils::split(raw_query, "&");

		for (auto pair : query_pairs)
		{
			auto query = utils::split_once(pair, "=");
			if (query)
			{
				auto [key, value] = query.value();
				result.params.insert_or_assign(utils::url_decode(key), utils::url_decode(value));
			}
			else
			{
				result.params.insert_or_assign(utils::url_decode(pair), "");
			}
		}
		return result;
	}

	std::vector<std::string> split_path(std::string_view path)
	{
		auto segments = utils::split(path, "/");
		return segments.empty() ? std::vector<std::string>{""} : segments;
	}

	match_result match_path(std::string_view pattern, std::string_view path)
	{
		match_result result{{}, true};
		auto pattern_segments = split_path(pattern);
		auto path_segments = split_path(path);

		if (pattern_segments.size() != path_segments.size())
		{
			result.matched = false;
			return result;
		}

		for (size_t i = 0; i < pattern_segments.size(); i++)
		{
			const auto pattern_seg = pattern_segments[i];
			const auto path_seg = path_segments[i];

			if (pattern_seg.starts_with(":"))
			{
				result.params.insert_or_assign(pattern_seg.substr(1), path_seg);
			}
			else
			{
				if (pattern_seg != path_seg)
				{
					result.params = {};
					result.matched = false;
					return result;
				}
			}
		}
		return result;
	}

} // namespace http