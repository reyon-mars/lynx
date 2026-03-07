#include <catch2/catch_test_macros.hpp>

#include "net/net_except.hpp"
#include "net/socket.hpp"
#include "net/socket_address.hpp"
#include "net/socket_config.hpp"

#include <cstddef>
#include <netinet/in.h>
#include <span>
#include <string>
#include <string_view>
#include <sys/_types/_ssize_t.h>
#include <thread>
#include <utility>
#include <vector>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

using namespace std::literals;

TEST_CASE("Socket lifecycle", "[socket]")
{
	net::socket_config cfg;

	SECTION("construction")
	{
		net::Socket s(cfg);
	}

	SECTION("move constructor")
	{
		net::Socket a(cfg);
		net::Socket b(std::move(a));
	}

	SECTION("move assignment")
	{
		net::Socket a(cfg);
		net::Socket b(cfg);
		b = std::move(a);
	}

	SECTION("self move assignment")
	{
		net::Socket s(cfg);
		s = std::move(s);
	}
}

TEST_CASE("Socket bind", "[socket][bind]")
{
	net::socket_config cfg;

	SECTION("valid bind")
	{
		net::Socket s(cfg);
		net::socket_address addr{"127.0.0.1", 9300};

		s.bind(addr);
	}

	SECTION("invalid ip")
	{
		net::Socket s(cfg);
		net::socket_address addr{"999.999.999.999", 0};

		CHECK_THROWS_AS(s.bind(addr), net::net_except);
	}

	SECTION("port already in use")
	{
		net::socket_address addr{"127.0.0.1", 9301};

		net::Socket s1(cfg);
		net::Socket s2(cfg);

		s1.bind(addr);

		CHECK_THROWS_AS(s2.bind(addr), net::net_except);
	}
}

TEST_CASE("Socket listen", "[socket][listen]")
{
	net::socket_config cfg;

	SECTION("listen after bind")
	{
		net::Socket s(cfg);
		net::socket_address addr{"127.0.0.1", 9302};

		s.bind(addr);
		s.listen();
	}
}

TEST_CASE("Socket accept", "[socket][accept]")
{
	net::socket_config cfg;
	net::socket_address addr{"127.0.0.1", 9303};

	net::Socket server(cfg);

	server.bind(addr);
	server.listen();

	std::thread client(
		[&]
		{
			int raw = ::socket(AF_INET, SOCK_STREAM, 0);

			sockaddr_in native = addr.to_sockaddr_in();

			::connect(raw, reinterpret_cast<sockaddr*>(&native), sizeof(native));

			::close(raw);
		});

	net::Socket conn = server.accept();

	client.join();
}

TEST_CASE("Socket send receive", "[socket][io]")
{
	net::socket_config cfg;
	net::socket_address addr{"127.0.0.1", 9304};

	net::Socket server(cfg);

	server.bind(addr);
	server.listen();

	std::string_view msg = "Hello Lynx!";
	std::string received;

	std::thread client(
		[&]
		{
			int raw = ::socket(AF_INET, SOCK_STREAM, 0);

			sockaddr_in native = addr.to_sockaddr_in();

			::connect(raw, reinterpret_cast<sockaddr*>(&native), sizeof(native));

			char buf[128]{};

			ssize_t n = ::recv(raw, buf, sizeof(buf), 0);

			if (n > 0)
				received.assign(buf, n);

			::close(raw);
		});

	net::Socket conn = server.accept();

	auto bytes = std::as_bytes(std::span(msg));

	ssize_t sent = conn.send(bytes);

	REQUIRE(sent == msg.size());

	client.join();

	REQUIRE(received == msg);
}

TEST_CASE("Socket large payload", "[socket][large]")
{
	net::socket_config cfg;
	net::socket_address addr{"127.0.0.1", 9305};

	net::Socket server(cfg);

	server.bind(addr);
	server.listen();

	std::vector<std::byte> payload(1 << 20);

	std::thread client(
		[&]
		{
			int raw = ::socket(AF_INET, SOCK_STREAM, 0);

			sockaddr_in native = addr.to_sockaddr_in();

			::connect(raw, reinterpret_cast<sockaddr*>(&native), sizeof(native));

			std::vector<std::byte> buffer(payload.size());

			size_t total = 0;

			while (total < buffer.size())
			{
				ssize_t r = ::recv(raw, reinterpret_cast<char*>(buffer.data()) + total, buffer.size() - total, 0);

				if (r <= 0)
					break;

				total += r;
			}

			REQUIRE(total == payload.size());

			::close(raw);
		});

	net::Socket conn = server.accept();

	ssize_t sent = conn.send(payload);

	REQUIRE(sent == payload.size());

	client.join();
}