#include "http/http_request.hpp"
#include "http/http_types.hpp"
#include "utils/string_utils.hpp"
#include <algorithm>
#include <cctype>
#include <cstddef>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace http
{
	void http_request::set_method(std::string method)
	{
		request_line_.method_ = std::move(method);
	}

	void http_request::set_uri(std::string uri)
	{
		request_line_.uri_ = std::move(uri);
	}

	void http_request::set_version(std::string version)
	{
		request_line_.version_ = std::move(version);
	}

	void http_request::add_header(std::string_view name, std::string value)
	{
		std::string normalized_name = utils::to_lower(name);
		headers_.insert_or_assign(normalized_name, std::move(value));
	}

	void http_request::remove_header(std::string_view name)
	{
		std::string normalized_name = utils::to_lower(name);
		headers_.erase(normalized_name);
	}

	bool http_request::has_header(std::string_view name) const
	{
		std::string normalized_name = utils::to_lower(name);
		return headers_.contains(normalized_name);
	}

	void http_request::set_body(std::string_view body)
	{
		if (body_.capacity() < body.size())
		{
			body_.reserve(body.size());
		}
		auto view = std::as_bytes(std::span(body));
		body_.assign(view.begin(), view.end());
		add_header("Content-Length", std::to_string(body_.size()));
	}

	void http_request::set_body(std::span<const std::byte> body)
	{
		if (body_.capacity() < body.size())
		{
			body_.reserve(body.size());
		}
		body_.assign(body.begin(), body.end());
		add_header("Content-Length", std::to_string(body_.size()));
	}

	void http_request::set_body(std::vector<std::byte> body)
	{
		body_ = std::move(body);
		add_header("Content-Length", std::to_string(body_.size()));
	}

	const std::string& http_request::method() const
	{
		return request_line_.method_;
	}

	const std::string& http_request::uri() const
	{
		return request_line_.uri_;
	}

	const std::string& http_request::version() const
	{
		return request_line_.version_;
	}

	std::optional<std::string_view> http_request::get_header(std::string_view name) const
	{
		auto it = headers_.find(utils::to_lower(name));
		if (it != headers_.end())
		{
			return it->second;
		}
		return std::nullopt;
	}

	const http_body& http_request::body() const
	{
		return body_;
	}

	bool http_request::has_body() const
	{
		return !body_.empty();
	}

	size_t http_request::content_length() const
	{
		return body_.size();
	}

} // namespace http