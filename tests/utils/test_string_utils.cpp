#include "http/http_types.hpp"
#include "utils/string_utils.hpp"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("String Trimming", "[utils][string]")
{
	REQUIRE(utils::trim("  hello  ") == "hello");
	REQUIRE(utils::trim("\n\t  tabs  \r") == "tabs");
	REQUIRE(utils::trim("no_spaces") == "no_spaces");
	REQUIRE(utils::trim("   ") == "");
}

TEST_CASE("String Splitting", "[utils][string]")
{
	SECTION("Split by single character")
	{
		auto parts = utils::split("api/v1/users", "/");
		REQUIRE(parts.size() == 3);
		REQUIRE(parts[0] == "api");
		REQUIRE(parts[1] == "v1");
		REQUIRE(parts[2] == "users");
	}

	SECTION("Split by multi-character delimeter (CRLF) ")
	{
		std::string_view request = "Line1\r\nLine2\r\nLine3\r\nLine4";
		auto lines = utils::split(request, http::CRLF);

		REQUIRE(lines.size() == 4);
		REQUIRE(lines[0] == "Line1");
		REQUIRE(lines[1] == "Line2");
		REQUIRE(lines[2] == "Line3");
		REQUIRE(lines[3] == "Line4");
	}
	SECTION("Empty or missing delimeters")
	{
		REQUIRE(utils::split("", ",").empty());
		auto no_match = utils::split("hello", "missing");
		REQUIRE(no_match.size() == 1);
		REQUIRE(no_match.front() == "hello");
	}
}

TEST_CASE("Split once (Key-Value parsing )", "[utils][string]")
{
	SECTION("Standard Header")
	{
		auto [key, value] = utils::split_once("Content-Type: text/html", ": ");
		REQUIRE(key == "Content-Type");
		REQUIRE(value == "text/html");
	}

	SECTION("Port numbers in Host headers")
	{
		// Should only split at the first colon
		auto [key, value] = utils::split_once("Host: localhost:8080", ": ");
		REQUIRE(key == "Host");
		REQUIRE(value == "localhost:8080");
	}
}

TEST_CASE("URL Decoding", "[utils][url]")
{
	SECTION("Standard characters")
	{
		REQUIRE(utils::url_decode("Hello%20World") == "Hello World");
		REQUIRE(utils::url_decode("C%2B%2B") == "C++");
	}

	SECTION("Form-encoded spaces")
	{
		REQUIRE(utils::url_decode("search+term") == "search term");
	}

	SECTION("Malformed inputs should be handled gracefully")
	{
		// Ensuring your pointer arithmetic (i+2) is safe
		REQUIRE(utils::url_decode("%") == "%");
		REQUIRE(utils::url_decode("%G1") == "%G1"); // Invalid hex
	}
}