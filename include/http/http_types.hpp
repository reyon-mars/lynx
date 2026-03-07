#pragma once

#include <cstddef>
#include <map>
#include <string>
#include <string_view>
#include <unordered_set>
#include <vector>

namespace http
{

	constexpr std::string_view CRLF = "\r\n";

	struct request_line
	{
		std::string method_;
		std::string uri_;
		std::string version_ = "HTTP/1.1";
	};

	struct status_line
	{
		std::string version_ = "HTTP/1.1";
		int status_code_ = 200;
		std::string reason_phrase_ = "OK";
	};

	using headers = std::map<std::string, std::string>;
	using http_body = std::vector<std::byte>;

	[[nodiscard]] inline bool is_valid_method(std::string_view method)
	{
		static const std::unordered_set<std::string_view>
			valid_methods{"GET", "POST", "PUT", "DELETE", "HEAD", "OPTIONS", "PATCH"};
		return valid_methods.contains(method);
	}
} // namespace http