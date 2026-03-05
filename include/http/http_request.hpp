#pragma once
#include <cstddef>
#include <map>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

constexpr size_t REQ_LINE_SIZE = 3;

namespace http
{
	class http_request
	{
	private:
		std::string method_;
		std::string uri_;
		std::string version_;
		std::map<std::string, std::string> headers_;
		std::vector<std::byte> body_;

	public:
		void set_method(std::string method);
		void set_uri(std::string uri);
		void set_version(std::string version);

		void add_header(std::string name, std::string value);
		void set_body(std::vector<std::byte> body);

		const std::string& method() const;
		const std::string& uri() const;
		const std::string& version() const;
		std::optional<std::string> get_header(std::string_view name) const;
		const std::vector<std::byte>& body() const;

		bool has_body() const;
		size_t content_length() const;


	};
} // namespace http