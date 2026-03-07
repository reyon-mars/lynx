#pragma once
#include "http/http_request.hpp"
#include "http/uri_parser.hpp"
#include <map>
#include <string>
namespace http
{
	struct request_context
	{
		const http::http_request& request;
		std::map<std::string, std::string> params;
		parsed_uri uri_info;
	};
} // namespace http