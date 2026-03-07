#include "http/uri_parser.hpp"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("URI Parser: Path and Query splitting", "[uri_parser]")
{

	SECTION("Basic path without query string")
	{
		auto res = http::parse_uri("/index.html");
		CHECK(res.path == "/index.html");
		CHECK(res.query.empty());
		CHECK(res.params.empty());
	}

	SECTION("Path with simple query string")
	{
		auto res = http::parse_uri("/search?q=lynx");
		CHECK(res.path == "/search");
		CHECK(res.query == "q=lynx");
		REQUIRE(res.params.size() == 1);
		CHECK(res.params.at("q") == "lynx");
	}

	SECTION("Path with multiple query parameters")
	{
		auto res = http::parse_uri("/api/v1/user?id=123&mode=dark&session=true");
		CHECK(res.path == "/api/v1/user");
		CHECK(res.params.at("id") == "123");
		CHECK(res.params.at("mode") == "dark");
		CHECK(res.params.at("session") == "true");
	}
}

TEST_CASE("URI Parser: Encoding and Special Characters", "[uri_parser]")
{

	SECTION("URL Decoding in path")
	{
		// Space encoded as %20
		auto res = http::parse_uri("/my%20documents/file.txt");
		CHECK(res.path == "/my documents/file.txt");
	}

	SECTION("URL Decoding in keys and values")
	{
		// 'C++' is often encoded as 'C%2B%2B' or 'C++' (the plus as space)
		auto res = http::parse_uri("/search?category=C%2B%2B&user=John+Doe");

		CHECK(res.params.at("category") == "C++");
		CHECK(res.params.at("user") == "John Doe");
	}

	SECTION("Complex encoded characters")
	{
		// Testing characters like #, &, and = inside values
		auto res = http::parse_uri("/test?data=%23%26%3D");
		CHECK(res.params.at("data") == "#&=");
	}
}

TEST_CASE("URI Parser: Edge Cases and Malformed Input", "[uri_parser]")
{

	SECTION("Empty query string (trailing question mark)")
	{
		auto res = http::parse_uri("/home?");
		CHECK(res.path == "/home");
		CHECK(res.query.empty());
		CHECK(res.params.empty());
	}

	SECTION("Parameters with no values")
	{
		// Common in flags like ?debug or ?print
		auto res = http::parse_uri("/list?debug&sort=asc");
		CHECK(res.params.count("debug"));
		CHECK(res.params.at("debug") == "");
		CHECK(res.params.at("sort") == "asc");
	}

	SECTION("Multiple delimiters (empty segments)")
	{
		auto res = http::parse_uri("/path?&&key==val&&");
		// Should handle empty segments from '&&' and '==' gracefully
		CHECK(res.params.at("key") == "=val"); // or however your split logic treats double '='
	}

	SECTION("Missing key or value")
	{
		auto res = http::parse_uri("/path?=value&key=");
		// Key is empty string
		CHECK(res.params.at("key") == "");
	}
}
TEST_CASE("URI Matcher: Static Path Matching", "[router]")
{
	SECTION("Exact match of single segment")
	{
		auto res = http::match_path("/about", "/about");
		CHECK(res.matched == true);
		CHECK(res.params.empty());
	}

	SECTION("Exact match of nested segments")
	{
		auto res = http::match_path("/api/v1/status", "/api/v1/status");
		CHECK(res.matched == true);
	}

	SECTION("Mismatch on segment value")
	{
		auto res = http::match_path("/users", "/admins");
		CHECK(res.matched == false);
	}

	SECTION("Mismatch on path length")
	{
		// Pattern is longer than path
		auto res = http::match_path("/api/v1/users", "/api/v1");
		CHECK(res.matched == false);

		// Path is longer than pattern
		auto res2 = http::match_path("/api/v1", "/api/v1/users");
		CHECK(res2.matched == false);
	}
}

TEST_CASE("URI Matcher: Dynamic Parameter Extraction", "[router]")
{
	SECTION("Single parameter in middle of path")
	{
		auto res = http::match_path("/user/:id/profile", "/user/42/profile");
		REQUIRE(res.matched == true);
		REQUIRE(res.params.count("id") == 1);
		CHECK(res.params.at("id") == "42");
	}

	SECTION("Multiple parameters")
	{
		auto res = http::match_path("/repo/:owner/:name", "/repo/lynx-project/core");
		REQUIRE(res.matched == true);
		CHECK(res.params.at("owner") == "lynx-project");
		CHECK(res.params.at("name") == "core");
	}

	SECTION("Trailing parameter")
	{
		auto res = http::match_path("/search/:term", "/search/cpp_concurrency");
		REQUIRE(res.matched == true);
		CHECK(res.params.at("term") == "cpp_concurrency");
	}
}

TEST_CASE("URI Matcher: Edge Cases", "[router]")
{
	SECTION("Root path matching")
	{
		auto res = http::match_path("/", "/");
		CHECK(res.matched == true);
	}

	SECTION("Empty segments in path (Slop handling)")
	{
		// If your split_path handles // gracefully, this should still match
		auto res = http::match_path("/api/:id", "/api//123");
		CHECK(res.matched == true);
		CHECK(res.params.at("id") == "123");
	}

	SECTION("Parameter name without value (Trailing slash)")
	{
		// If the path ends in a slash, does :id become empty?
		auto res = http::match_path("/user/:id", "/user/");
		// Based on your split_path logic returning {""}, this should match
		if (res.matched)
		{
			CHECK(res.params.at("id") == "");
		}
	}
}