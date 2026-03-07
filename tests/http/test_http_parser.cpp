#include "http/http_parser.hpp"
#include "http/http_request.hpp"
#include <catch2/catch_test_macros.hpp>
#include <cstddef>
#include <span>
#include <string>
#include <string_view>

using namespace http;

/**
 * @brief Helper to convert string_view to a byte span for the parser.
 */
static std::span<const std::byte> to_bytes(std::string_view s)
{
	return {reinterpret_cast<const std::byte*>(s.data()), s.size()};
}

TEST_CASE("HTTP Parser - Request Line Parsing", "[http][parser]")
{
	http_parser parser;

	SECTION("Valid GET request")
	{
		std::string_view req = "GET /index.html HTTP/1.1\r\n\r\n";
		auto result = parser.feed(to_bytes(req));

		REQUIRE(result == http_parser::parse_result::complete);

		auto request = parser.extract();
		REQUIRE(request.has_value());
		REQUIRE(request->method() == "GET");
		REQUIRE(request->uri() == "/index.html");
		REQUIRE(request->version() == "HTTP/1.1");
	}

	SECTION("Invalid request line")
	{
		std::string_view req = "INVALID_REQUEST\r\n";
		auto result = parser.feed(to_bytes(req));
		REQUIRE(result == http_parser::parse_result::error);
	}

	SECTION("Partial request line")
	{
		std::string_view part1 = "GET /ind";
		auto result = parser.feed(to_bytes(part1));
		REQUIRE(result == http_parser::parse_result::incomplete_data);
	}
}

TEST_CASE("HTTP Parser - Header Parsing", "[http][parser][headers]")
{
	http_parser parser;

	SECTION("Single header")
	{
		std::string_view req = "GET / HTTP/1.1\r\nHost: example.com\r\n\r\n";
		REQUIRE(parser.feed(to_bytes(req)) == http_parser::parse_result::complete);

		auto request = parser.extract();
		REQUIRE(request->has_header("host"));

		auto v = request->get_header("host");
		REQUIRE(v.has_value());
		REQUIRE(*v == "example.com");
	}

	SECTION("Multiple headers")
	{
		std::string_view req = "GET / HTTP/1.1\r\n"
							   "Host: example.com\r\n"
							   "User-Agent: Test\r\n"
							   "Accept: */*\r\n\r\n";

		parser.feed(to_bytes(req));
		auto request = parser.extract();

		REQUIRE(request->has_header("host"));
		REQUIRE(request->has_header("user-agent"));
		REQUIRE(request->has_header("accept"));
		REQUIRE(request->get_header("accept").value() == "*/*");
	}

	SECTION("Header whitespace trimming")
	{
		std::string_view req = "GET / HTTP/1.1\r\nHost:    example.com   \r\n\r\n";
		parser.feed(to_bytes(req));
		auto request = parser.extract();
		REQUIRE(request->get_header("host").value() == "example.com");
	}

	SECTION("Header case normalization")
	{
		parser.feed(to_bytes("GET / HTTP/1.1\r\nHost: example.com\r\n\r\n"));
		auto req = parser.extract();
		REQUIRE(req->get_header("HOST").value() == "example.com");
		REQUIRE(req->get_header("host").value() == "example.com");
	}

	SECTION("Header overwrite behavior")
	{
		parser.feed(to_bytes("GET / HTTP/1.1\r\nHost: first.com\r\nHost: second.com\r\n\r\n"));
		auto req = parser.extract();
		REQUIRE(req->get_header("host").value() == "second.com");
	}

	SECTION("Invalid header format")
	{
		std::string_view req = "GET / HTTP/1.1\r\nInvalidHeader\r\n\r\n";
		auto result = parser.feed(to_bytes(req));
		REQUIRE(result == http_parser::parse_result::error);
	}
}

TEST_CASE("HTTP Parser - Body Parsing", "[http][parser][body]")
{
	http_parser parser;

	SECTION("Simple body")
	{
		std::string_view req = "POST /data HTTP/1.1\r\nContent-Length: 5\r\n\r\nHello";
		parser.feed(to_bytes(req));

		auto request_opt = parser.extract();
		REQUIRE(request_opt.has_value());

		auto& request = *request_opt;
		auto cl_header = request.get_header("content-length");
		auto content_len = request.content_length();

		INFO("Raw Header String: " << cl_header.value_or("MISSING"));
		CAPTURE(content_len);

		REQUIRE(request.has_body());
		REQUIRE(content_len == 5);
	}

	SECTION("Partial body feed")
	{
		parser.feed(to_bytes("POST / HTTP/1.1\r\nContent-Length: 5\r\n\r\nHe"));
		REQUIRE_FALSE(parser.is_complete());

		auto r2 = parser.feed(to_bytes("llo"));
		REQUIRE(r2 == http_parser::parse_result::complete);
		REQUIRE(parser.is_complete());

		auto request = parser.extract();
		REQUIRE(request->content_length() == 5);
	}

	SECTION("Zero content length")
	{
		parser.feed(to_bytes("POST / HTTP/1.1\r\nContent-Length: 0\r\n\r\n"));
		auto request = parser.extract();
		REQUIRE_FALSE(request->has_body());
	}

	SECTION("Invalid content length")
	{
		auto r = parser.feed(to_bytes("POST / HTTP/1.1\r\nContent-Length: abc\r\n\r\n"));
		REQUIRE(r == http_parser::parse_result::error);
	}
}

TEST_CASE("HTTP Parser - Incremental & Fragmented Input", "[http][parser][stream]")
{
	http_parser parser;

	SECTION("Request line split across feeds")
	{
		parser.feed(to_bytes("GET / HT"));
		auto r = parser.feed(to_bytes("TP/1.1\r\n\r\n"));
		REQUIRE(r == http_parser::parse_result::complete);
		REQUIRE(parser.is_complete());
	}

	SECTION("CRLF boundary split")
	{
		parser.feed(to_bytes("GET / HTTP/1.1\r"));
		auto r = parser.feed(to_bytes("\n\r\n"));
		REQUIRE(r == http_parser::parse_result::complete);
	}

	SECTION("One byte at a time")
	{
		std::string req = "GET / HTTP/1.1\r\nHost: a\r\n\r\n";
		for (char c : req)
		{
			parser.feed(to_bytes(std::string(1, c)));
		}
		REQUIRE(parser.is_complete());
		auto extracted = parser.extract();
		REQUIRE(extracted->method() == "GET");
	}
}

TEST_CASE("HTTP Parser - Multi-Request and State Management", "[http][parser]")
{
	http_parser parser;

	SECTION("Multiple Requests In Buffer")
	{
		std::string_view req = "GET /1 HTTP/1.1\r\n\r\nGET /2 HTTP/1.1\r\n\r\n";
		parser.feed(to_bytes(req));

		auto r1 = parser.extract();
		REQUIRE(r1.has_value());
		REQUIRE(r1->uri() == "/1");

		parser.feed({}); // Trigger parsing of remaining buffer
		auto r2 = parser.extract();
		REQUIRE(r2.has_value());
		REQUIRE(r2->uri() == "/2");
	}

	SECTION("Reset Behavior")
	{
		parser.feed(to_bytes("GET / HTTP/1.1\r\n\r\n"));
		REQUIRE(parser.extract().has_value());

		parser.reset();
		auto r = parser.feed(to_bytes("GET /again HTTP/1.1\r\n\r\n"));
		REQUIRE(r == http_parser::parse_result::complete);
		REQUIRE(parser.extract()->uri() == "/again");
	}

	SECTION("Extract empties parser state")
	{
		parser.feed(to_bytes("GET / HTTP/1.1\r\n\r\n"));
		parser.extract();
		REQUIRE_FALSE(parser.is_complete());
	}
}