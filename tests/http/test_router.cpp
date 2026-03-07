#include <catch2/catch_test_macros.hpp>

#include "http/http_request.hpp"
#include "http/http_response.hpp"
#include "http/request_context.hpp"
#include "http/router.hpp"

#include <map>
#include <string>
#include <string_view>
#include <thread>
#include <utility>
#include <vector>

using namespace std::literals;

namespace
{

	http::http_response make_response(std::string_view body)
	{
		http::http_response r;
		r.set_status(200);
		r.set_body(body);
		return r;
	}

	http::request_context make_context(const http::http_request& req, std::map<std::string, std::string> params = {})
	{
		http::request_context ctx{.request = req, .params = std::move(params), .uri_info = {}};
		return ctx;
	}

} // namespace

TEST_CASE("Router initial state", "[router][lifecycle]")
{
	http::router r;

	SECTION("empty router has no match")
	{
		REQUIRE_FALSE(r.match("GET", "/"));
	}

	SECTION("multiple unmatched requests")
	{
		REQUIRE_FALSE(r.match("GET", "/a"));
		REQUIRE_FALSE(r.match("POST", "/a"));
		REQUIRE_FALSE(r.match("DELETE", "/a"));
	}
}

TEST_CASE("Basic route registration and invocation", "[router][basic]")
{
	http::router r;

	r.add_route("GET",
				"/hello",
				[](const http::request_context&)
				{
					return make_response("world");
				});

	auto match = r.match("GET", "/hello");

	REQUIRE(match);
	REQUIRE(match->handler != nullptr);

	http::http_request req;
	auto ctx = make_context(req, match->params);

	auto res = (*match->handler)(ctx);

	REQUIRE(res.status_code() == 200);
}

TEST_CASE("HTTP method filtering", "[router][method]")
{
	http::router r;

	r.add_route("GET",
				"/resource",
				[](const http::request_context&)
				{
					return make_response("GET");
				});

	r.add_route("POST",
				"/resource",
				[](const http::request_context&)
				{
					return make_response("POST");
				});

	SECTION("GET route matches")
	{
		REQUIRE(r.match("GET", "/resource"));
	}

	SECTION("POST route matches")
	{
		REQUIRE(r.match("POST", "/resource"));
	}

	SECTION("PUT does not match")
	{
		REQUIRE_FALSE(r.match("PUT", "/resource"));
	}
}

TEST_CASE("Exact path matching", "[router][path]")
{
	http::router r;

	r.add_route("GET",
				"/alpha",
				[](const http::request_context&)
				{
					return make_response("alpha");
				});

	SECTION("exact match")
	{
		REQUIRE(r.match("GET", "/alpha"));
	}

	SECTION("prefix mismatch")
	{
		REQUIRE_FALSE(r.match("GET", "/alph"));
	}

	SECTION("suffix mismatch")
	{
		REQUIRE_FALSE(r.match("GET", "/alpha/beta"));
	}

	SECTION("case sensitive")
	{
		REQUIRE_FALSE(r.match("GET", "/Alpha"));
	}
}

TEST_CASE("Root path handling", "[router][root]")
{
	http::router r;

	r.add_route("GET",
				"/",
				[](const http::request_context&)
				{
					return make_response("root");
				});

	REQUIRE(r.match("GET", "/"));

	REQUIRE(r.match("GET", ""));
}

TEST_CASE("Single parameter extraction", "[router][params]")
{
	http::router r;

	r.add_route("GET",
				"/users/{id}",
				[](const http::request_context& ctx)
				{
					REQUIRE(ctx.params.count("id") == 1);
					return make_response("ok");
				});

	auto match = r.match("GET", "/users/42");

	REQUIRE(match);
	REQUIRE(match->params.size() == 1);
	REQUIRE(match->params.at("id") == "42");
}

TEST_CASE("Multiple parameter extraction", "[router][params][multi]")
{
	http::router r;

	r.add_route("GET",
				"/posts/{category}/{slug}",
				[](const http::request_context& ctx)
				{
					REQUIRE(ctx.params.count("category") == 1);
					REQUIRE(ctx.params.count("slug") == 1);
					return make_response("ok");
				});

	auto m = r.match("GET", "/posts/cpp/router-design");

	REQUIRE(m);
	REQUIRE(m->params.at("category") == "cpp");
	REQUIRE(m->params.at("slug") == "router-design");
}

TEST_CASE("Deep parameter paths", "[router][params][deep]")
{
	http::router r;

	r.add_route("GET",
				"/a/{b}/c/{d}/e/{f}",
				[](const http::request_context& ctx)
				{
					REQUIRE(ctx.params.size() == 3);
					return make_response("ok");
				});

	auto m = r.match("GET", "/a/x/c/y/e/z");

	REQUIRE(m);
	REQUIRE(m->params.at("b") == "x");
	REQUIRE(m->params.at("d") == "y");
	REQUIRE(m->params.at("f") == "z");
}

TEST_CASE("Route ordering precedence", "[router][priority]")
{
	http::router r;

	r.add_route("GET",
				"/users/{id}",
				[](const http::request_context&)
				{
					return make_response("param");
				});

	r.add_route("GET",
				"/users/profile",
				[](const http::request_context&)
				{
					return make_response("profile");
				});

	auto m = r.match("GET", "/users/profile");

	REQUIRE(m);

	http::http_request req;
	auto ctx = make_context(req, m->params);

	auto res = (*m->handler)(ctx);

	REQUIRE(res.status_code() == 200);
}

TEST_CASE("Duplicate routes", "[router][duplicate]")
{
	http::router r;

	r.add_route("GET",
				"/dup",
				[](const http::request_context&)
				{
					return make_response("first");
				});

	r.add_route("GET",
				"/dup",
				[](const http::request_context&)
				{
					return make_response("second");
				});

	auto m = r.match("GET", "/dup");

	REQUIRE(m);
}

TEST_CASE("Trailing slash behavior", "[router][slash]")
{
	http::router r;

	r.add_route("GET",
				"/docs",
				[](const http::request_context&)
				{
					return make_response("docs");
				});

	REQUIRE(r.match("GET", "/docs"));

	REQUIRE_FALSE(r.match("GET", "/docs/"));
}

TEST_CASE("Large route table stress test", "[router][stress]")
{
	http::router r;

	const int count = 1000;

	for (int i = 0; i < count; i++)
	{
		r.add_route("GET",
					"/route" + std::to_string(i),
					[](const http::request_context&)
					{
						return make_response("ok");
					});
	}

	for (int i = 0; i < count; i++)
	{
		auto m = r.match("GET", "/route" + std::to_string(i));
		REQUIRE(m);
	}
}

TEST_CASE("Handler invocation side effects", "[router][handler]")
{
	http::router r;

	bool called = false;

	r.add_route("GET",
				"/call",
				[&](const http::request_context&)
				{
					called = true;
					return make_response("ok");
				});

	auto m = r.match("GET", "/call");

	REQUIRE(m);

	http::http_request req;
	auto ctx = make_context(req, m->params);

	(*m->handler)(ctx);

	REQUIRE(called);
}

TEST_CASE("Unmatched routes", "[router][negative]")
{
	http::router r;

	r.add_route("GET",
				"/a",
				[](const http::request_context&)
				{
					return make_response("a");
				});

	REQUIRE_FALSE(r.match("GET", "/b"));
	REQUIRE_FALSE(r.match("POST", "/a"));
	REQUIRE_FALSE(r.match("GET", "/a/b"));
}

TEST_CASE("Very long paths", "[router][edge]")
{
	http::router r;

	std::string long_segment(1024, 'x');

	r.add_route("GET",
				"/" + long_segment,
				[](const http::request_context&)
				{
					return make_response("long");
				});

	REQUIRE(r.match("GET", "/" + long_segment));
}

TEST_CASE("Concurrent match safety", "[router][concurrency]")
{
	http::router r;

	r.add_route("GET",
				"/a",
				[](const http::request_context&)
				{
					return make_response("A");
				});

	r.add_route("GET",
				"/b",
				[](const http::request_context&)
				{
					return make_response("B");
				});

	std::vector<std::thread> threads;

	for (int i = 0; i < 16; i++)
	{
		threads.emplace_back(
			[&]()
			{
				for (int j = 0; j < 1000; j++)
				{
					REQUIRE(r.match("GET", "/a"));
					REQUIRE(r.match("GET", "/b"));
				}
			});
	}

	for (auto& t : threads)
		t.join();
}