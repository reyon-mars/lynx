#include "http/http_parser.hpp"
#include "http/http_request.hpp"
#include "http/http_types.hpp"
#include "utils/string_utils.hpp"
#include <algorithm>
#include <cstddef>
#include <cstring>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <utility>

namespace http
{
	std::string_view http_parser::unparsed_view() const
	{
		if (parsed_offset_ >= buffer_.size())
		{
			return {};
		}
		return std::string_view(reinterpret_cast<const char*>(buffer_.data() + parsed_offset_),
								buffer_.size() - parsed_offset_);
	}

	std::optional<size_t> http_parser::find_crlf() const
	{
		auto view = unparsed_view();
		auto pos = view.find(CRLF);

		if (pos == std::string_view::npos)
		{
			return std::nullopt;
		}
		return pos;
	}

	bool http_parser::parse_request_line()
	{
		auto pos = find_crlf();
		if (!pos)
		{
			return false;
		}

		size_t end = pos.value();
		std::string_view line = unparsed_view().substr(0, end);
		parsed_offset_ += (end + CRLF.size());

		auto s1 = line.find(' ');
		auto s2 = line.find(' ', (s1 + 1));

		if (s1 == std::string_view::npos || s2 == std::string_view::npos)
		{
			state_ = state::Error;
			return false;
		}

		std::string_view method = line.substr(0, s1);
		if (!is_valid_method(method))
		{
			state_ = state::Error;
			return false;
		}

		current_request_.set_method(std::string(method));
		current_request_.set_uri(std::string(line.substr(s1 + 1, s2 - s1 - 1)));
		current_request_.set_version(std::string(line.substr(s2 + 1)));

		state_ = state::Headers;

		return true;
	}

	bool http_parser::parse_header()
	{
		auto pos = find_crlf();
		if (!pos)
		{
			return false;
		}

		size_t end = pos.value();
		std::string_view line = unparsed_view().substr(0, end);

		parsed_offset_ += (end + CRLF.size());
		if (line.empty())
		{
			auto header = current_request_.get_header("Content-Length");
			if (!header)
			{
				state_ = state::Done;
				return true;
			}
			auto parsed = utils::to_int(utils::trim(header.value()));
			if (!parsed)
			{
				state_ = state::Error;
				return false;
			}

			auto content_length = parsed.value();

			if (content_length < 0)
			{
				state_ = state::Error;
				return false;
			}

			content_length_ = static_cast<size_t>(content_length);
			state_ = content_length > 0 ? state::Body : state::Done;
			return true;
		}

		auto result = utils::split_once(line, ":");

		if (!result)
		{
			state_ = state::Error;
			return false;
		}

		auto [name, value] = result.value();

		current_request_.add_header(std::string(utils::trim(name)), std::string(utils::trim(value)));

		return true;
	}

	bool http_parser::parse_body()
	{
		if (content_length_ == 0)
		{
			state_ = state::Done;
			return true;
		}

		auto view = unparsed_view();
		if (view.size() < content_length_)
		{
			return false;
		}

		current_request_.set_body(std::span(buffer_.data() + parsed_offset_, content_length_));
		parsed_offset_ += content_length_;

		state_ = state::Done;
		return true;
	}

	bool http_parser::is_complete() const
	{
		return state_ == state::Done;
	}

	http_parser::parse_result http_parser::feed(std::span<const std::byte> data)
	{
		if (data.size() > MAX_HEADER_SIZE)
		{
			state_ = state::Error;
			return parse_result::error;
		}

		if (buffer_.capacity() < (data.size() + buffer_.size()))
		{
			buffer_.reserve(std::max((data.size() + buffer_.size()), buffer_.capacity() * 2));
		}
		buffer_.insert(buffer_.end(), data.begin(), data.end());

		bool progress = true;
		while (progress)
		{
			switch (state_)
			{
			case state::Request_line:
				progress = parse_request_line();
				break;

			case state::Headers:
				progress = parse_header();
				break;

			case state::Body:
				progress = parse_body();
				break;

			case state::Done:
				return parse_result::complete;

			case state::Error:
				return parse_result::error;

			default:
				progress = false;
				break;
			}
			if (state_ == state::Error)
			{
				return parse_result::error;
			}
		}
		return parse_result::incomplete_data;
	}

	std::optional<http_request> http_parser::extract()
	{
		if (state_ == state::Done)
		{
			std::optional<http_request> extracted_request = std::move(current_request_);
			reset();
			return extracted_request;
		}
		return std::nullopt;
	}

	void http_parser::reset()
	{
		state_ = state::Request_line;
		current_request_ = http_request{};
		content_length_ = 0;
		compact_buffer();
	}

	void http_parser::compact_buffer()
	{
		if (parsed_offset_ == 0)
		{
			return;
		}
		if (parsed_offset_ >= buffer_.size())
		{
			buffer_.clear();
		}
		else
		{
			std::memmove(buffer_.data(), buffer_.data() + parsed_offset_, buffer_.size() - parsed_offset_);
			buffer_.resize(buffer_.size() - parsed_offset_);
		}
		parsed_offset_ = 0;
	}
} // namespace http