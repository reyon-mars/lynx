#pragma once
#include "http/http_response.hpp"
#include "http/request_context.hpp"
#include "utils/function.hpp"
#include <map>
#include <optional>
#include <string>
#include <string_view>
#include <vector>
namespace http
{
	class router
	{
	public:
		using handler_function = utils::function<http::http_response(const request_context&)>;

		struct matched_route
		{
			const handler_function* handler;
			std::map<std::string, std::string> params;
		};

	private:
		struct route
		{
			std::string method;
			std::string pattern;
			handler_function handler;
		};
		std::vector<route> routes_;

	public:
		void add_route(std::string_view method, std::string_view pattern, handler_function&& handler);
		std::optional<matched_route> match(std::string_view method, std::string_view path) const;
	};
} // namespace http