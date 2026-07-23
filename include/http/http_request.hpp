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
		[[nodiscard]] bool has_header(std::string_view name) const;

		void set_body(std::string_view body);
		void set_body(std::span<const std::byte> body);
		void set_body(std::vector<std::byte> body);

		[[nodiscard]] const std::string& method() const;
		[[nodiscard]] const std::string& uri() const;
		[[nodiscard]] const std::string& version() const;
		[[nodiscard]] std::optional<std::string_view> get_header(std::string_view name) const;
		[[nodiscard]] const std::vector<std::byte>& body() const;

		[[nodiscard]] bool has_body() const;
		[[nodiscard]] bool has_valid_method() const;
		[[nodiscard]] size_t content_length() const;
	};
} // namespace http