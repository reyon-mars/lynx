#include "http/http_response.hpp"
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

	void http_response::set_status_code(int status_code)
	{
		status_code_ = status_code;
		reason_phrase_.assign(get_reason_phrase(status_code));
	}

	void http_response::set_header(std::string name, std::string value)
	{
		std::ranges::transform(name,
							   name.begin(),
							   [](unsigned char c)
							   {
								   return static_cast<char>(std::tolower(c));
							   });

		headers_.insert_or_assign(std::move(name), std::move(value));
	}

	void http_response::set_body(std::string body)
	{
		auto view = std::as_bytes(std::span(body));
		body_.assign(view.begin(), view.end());
	}

	void http_response::set_body(std::vector<std::byte> body)
	{
		body_ = std::move(body);
	}

	std::vector<std::byte> http_response::serialize()
	{
		std::ostringstream ss;
		ss << version_ << " " << status_code_ << " " << reason_phrase_ << " " << "\r\n";

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