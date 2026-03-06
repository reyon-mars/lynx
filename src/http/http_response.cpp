#include "http/http_response.hpp"
#include "utils/string_utils.hpp"
#include <algorithm>
#include <cstddef>
#include <cstring>
#include <map>
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
		static const std::map<int, std::string> reasons = {{200, "OK"},
														   {201, "Created"},
														   {204, "No Content"},
														   {400, "Bad Request"},
														   {401, "Unauthorized"},
														   {403, "Forbidden"},
														   {404, "Not Found"},
														   {405, "Method Not Allowed"},
														   {500, "Internal Server Error"},
														   {501, "Not Implemented"},
														   {503, "Service Unavailable"}};
		auto it = reasons.find(code);

		return (it != reasons.end()) ? it->second : "Unknown";
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
	}

	std::vector<std::byte> http_response::serialize()
	{
		std::ostringstream ss;
		ss << status_line_.version_ << " " << status_line_.status_code_ << " " << status_line_.reason_phrase_ << " "
		   << "\r\n";

		for (const auto& [name, value] : headers_)
		{
			ss << name << ": " << value << "\r\n";
		}
		ss << "\r\n";

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

} // namespace http