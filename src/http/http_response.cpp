#include "http/http_response.hpp"
#include "http/http_types.hpp"
#include "utils/string_utils.hpp"
#include <cstddef>
#include <cstring>
#include <span>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace http
{

	std::string_view get_reason_phrase(int code)
	{
		switch (code)
		{
			// 2xx Success
		case 200:
			return "OK";
		case 201:
			return "Created";
		case 202:
			return "Accepted";
		case 204:
			return "No Content";

		// 3xx Redirection
		case 301:
			return "Moved Permanently";
		case 302:
			return "Found";

		// 4xx Client Errors
		case 400:
			return "Bad Request";
		case 401:
			return "Unauthorized";
		case 403:
			return "Forbidden";
		case 404:
			return "Not Found";
		case 405:
			return "Method Not Allowed";

		// 5xx Server Errors
		case 500:
			return "Internal Server Error";
		case 501:
			return "Not Implemented";
		case 503:
			return "Service Unavailable";

		default:
			return "Unknown";
		}
	}

	void http_response::set_status(int status_code)
	{
		status_line_.status_code_ = status_code;
		status_line_.reason_phrase_.assign(get_reason_phrase(status_code));
	}

	int http_response::status_code() const
	{
		return status_line_.status_code_;
	}

	std::string_view http_response::reason_phrase() const
	{
		return status_line_.reason_phrase_;
	}

	void http_response::set_header(std::string_view name, std::string value)
	{
		std::string normalized_name = utils::to_lower(name);

		headers_.insert_or_assign(std::move(normalized_name), std::move(value));
	}

	void http_response::remove_header(std::string_view name)
	{
		std::string normalized_name = utils::to_lower(name);

		headers_.erase(normalized_name);
	}

	bool http_response::has_header(std::string_view name) const
	{
		std::string normalized_name = utils::to_lower(name);
		return headers_.contains(normalized_name);
	}

	void http_response::set_body(std::string_view body)
	{
		auto view = std::as_bytes(std::span(body));
		body_.assign(view.begin(), view.end());
		set_header("Content-Length", std::to_string(body_.size()));
	}

	void http_response::set_body(std::vector<std::byte> body)
	{
		body_ = std::move(body);
		set_header("Content-Length", std::to_string(body_.size()));
	}

	const http_body& http_response::body() const
	{
		return body_;
	}

	std::vector<std::byte> http_response::serialize() const
	{
		std::ostringstream ss;
		ss << status_line_.version_ << " " << status_line_.status_code_ << " " << status_line_.reason_phrase_ << CRLF;

		for (const auto& [name, value] : headers_)
		{
			ss << name << ": " << value << CRLF;
		}
		ss << CRLF;

		auto header_str = std::move(ss).str();
		std::vector<std::byte> serial_data;
		serial_data.reserve(header_str.size() + body_.size());

		for (auto data : header_str)
		{
			serial_data.push_back(static_cast<std::byte>(data));
		}
		serial_data.insert(serial_data.end(), body_.begin(), body_.end());
		return serial_data;
	}

	bool http_response::is_error() const
	{
		return status_line_.status_code_ >= 400;
	}

} // namespace http