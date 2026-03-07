#pragma once
#include "http/http_types.hpp"
#include <cstddef>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace http
{
	class http_request
	{
	private:
		request_line request_line_;
		headers headers_;
		http_body body_;

	public:
		http_request() = default;

		void set_method(std::string method);
		void set_uri(std::string uri);
		void set_version(std::string version);

		void add_header(std::string_view name, std::string value);
		void remove_header(std::string_view name);
		bool has_header(std::string_view name) const;

		void set_body(std::string_view body);
		void set_body(std::span<const std::byte> body);
		void set_body(std::vector<std::byte> body);

		const std::string& method() const;
		const std::string& uri() const;
		const std::string& version() const;
		std::optional<std::string_view> get_header(std::string_view name) const;
		const std::vector<std::byte>& body() const;

		bool has_body() const;
		size_t content_length() const;
	};
} // namespace http