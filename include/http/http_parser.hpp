#pragma once
#include "http/http_request.hpp"
#include <cstddef>
#include <optional>
#include <span>
#include <string_view>
#include <vector>
namespace http
{
	class http_parser
	{
	public:
		enum class parse_result
		{
			incomplete_data,
			complete,
			error
		};
		enum class state
		{
			Request_line,
			Headers,
			Body,
			Done,
			Error
		};

	private:
		static constexpr size_t MAX_HEADER_SIZE = 8192;

		std::vector<std::byte> buffer_;
		size_t parsed_offset_ = 0;
		http_request current_request_;

		state state_ = state::Request_line;

		size_t content_length_ = 0;

		void compact_buffer();
		std::string_view unparsed_view() const;

		std::optional<size_t> find_crlf() const;

		bool parse_request_line();
		bool parse_header();
		bool parse_body();

	public:
		parse_result feed(std::span<const std::byte> data);
		bool is_complete() const;
		std::optional<http_request> extract();

		void reset();
	};
} // namespace http