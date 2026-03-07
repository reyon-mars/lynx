#include "http/router.hpp"
#include "http/uri_parser.hpp"
#include <optional>
#include <string>
#include <string_view>
#include <utility>

namespace http
{
	void router::add_route(std::string_view method, std::string_view pattern, handler_function&& handler)
	{
		routes_.emplace_back(route{std::string(method), std::string(pattern), std::move(handler)});
	}

	std::optional<router::matched_route> router::match(std::string_view method, std::string_view path) const
	{
		for (const auto& route : routes_)
		{
			if (route.method != method)
			{
				continue;
			}
			match_result result = match_path(route.pattern, path);

			if (result.matched)
			{
				return matched_route{&route.handler, std::move(result.params)};
			}
		}
		return std::nullopt;
	}
} // namespace http