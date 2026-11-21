#include <catch2/catch_test_macros.hpp>

// #include "net/net_except.hpp"
#include "net/socket.hpp"
#include "net/socket_address.hpp"
#include "net/socket_config.hpp"
#include "net/socket_type.hpp"

#include <cstddef>
#include <netinet/in.h>
#include <span>
#include <string>
#include <string_view>
#include <thread>
#include <unistd.h>
#include <utility>
#include <vector>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

using namespace std::literals;

TEST_CASE("socket lifecycle", "[socket]") {
  lynx::net::socket_config cfg;

  SECTION("construction") {
    auto r = lynx::net::socket::create(cfg); // Ask for a socket
    REQUIRE(r.has_value());                  // Check that it worked
    lynx::net::socket s = std::move(*r);     // take ownership
    REQUIRE(s.valid());                      // Check that the socket is usable:

    // lynx::net::socket s(cfg);
  }

  SECTION("move constructor") {
    auto ra = lynx::net::socket::create(cfg);
    REQUIRE(ra.has_value());

    lynx::net::socket b(std::move(*ra));

    REQUIRE(b.valid());
    REQUIRE_FALSE(ra->valid());

    // lynx::net::socket a(cfg);
    // lynx::net::socket b(std::move(a));
  }

  SECTION("move assignment") {
    auto ra = lynx::net::socket::create(cfg);
    auto rb = lynx::net::socket::create(cfg);

    REQUIRE(ra.has_value());
    REQUIRE(rb.has_value());
    *rb = std::move(*ra);

    REQUIRE(rb->valid());
    REQUIRE_FALSE(ra->valid());

    // lynx::net::socket a(cfg);
    // lynx::net::socket b(cfg);
    // b = std::move(a);
  }

  SECTION("self move assignment") {

    auto r = lynx::net::socket::create(cfg);
    REQUIRE(r.has_value());

    lynx::net::socket &ref = *r;
    *r = std::move(ref);

    REQUIRE(r->valid());

    // lynx::net::socket s(cfg);
    // s = std::move(s);
  }
}

TEST_CASE("socket bind", "[socket][bind]") {
  lynx::net::socket_config cfg;

  SECTION("valid bind") {
    auto r = lynx::net::socket::create(cfg);
    REQUIRE(r.has_value());

    lynx::net::socket_address addr{"127.0.0.1", 9300};

    auto result = r->bind(addr);
    REQUIRE(result.has_value());

    // lynx::net::socket s(cfg);
    // lynx::net::socket_address addr{"127.0.0.1", 9300};

    // s.bind(addr);
  }

  // SECTION("invalid ip") {
  //   lynx::net::socket s(cfg);
  //   lynx::net::socket_address addr{"999.999.999.999", 0};

  //   CHECK_THROWS_AS(s.bind(addr), lynx::net::net_except);
  // }

  SECTION("port already in use") {
    auto r1 = lynx::net::socket::create(cfg);
    auto r2 = lynx::net::socket::create(cfg);

    REQUIRE(r1.has_value());
    REQUIRE(r2.has_value());

    lynx::net::socket_address addr{"127.0.0.1", 9300};

    auto first = r1->bind(addr);
    REQUIRE(first.has_value());

    auto second = r2->bind(addr);
    REQUIRE_FALSE(second.has_value());

    REQUIRE(second.error().operation == lynx::net::socket_operation::bind);

    // lynx::net::socket_address addr{"127.0.0.1", 9301};

    // lynx::net::socket s1(cfg);
    // lynx::net::socket s2(cfg);

    // s1.bind(addr);

    // CHECK_THROWS_AS(s2.bind(addr), lynx::net::net_except);
  }
}

TEST_CASE("socket listen", "[socket][listen]") {
  lynx::net::socket_config cfg;

  SECTION("listen after bind") {
    auto r = lynx::net::socket::create(cfg);
    REQUIRE(r.has_value());

    lynx::net::socket_address addr{"127.0.0.1", 9302};
    REQUIRE(r->bind(addr).has_value());
    REQUIRE(r->listen().has_value());

    // lynx::net::socket s(cfg);
    // lynx::net::socket_address addr{"127.0.0.1", 9302};

    // s.bind(addr);
    // s.listen();
  }
}

TEST_CASE("socket accept", "[socket][accept]") {
  lynx::net::socket_config cfg;
  lynx::net::socket_address addr{"127.0.0.1", 9303};

  // lynx::net::socket server(cfg);
  auto rs = lynx::net::socket::create(cfg);
  REQUIRE(rs.has_value());

  // server.bind(addr);
  REQUIRE(rs->bind(addr).has_value());
  // server.listen();
  REQUIRE(rs->listen().has_value());

  int connect_result = -1;

  std::thread client([&] {
    int raw = ::socket(AF_INET, SOCK_STREAM, 0);
    if (raw < 0)
      return;

    sockaddr_in native = addr.to_sockaddr_in();

    // ::connect(raw, reinterpret_cast<sockaddr *>(&native), sizeof(native));
    connect_result =
        ::connect(raw, reinterpret_cast<sockaddr *>(&native), sizeof(native));

    ::close(raw);
  });

  // lynx::net::socket conn = server.accept();
  auto conn = rs->accept();

  // client.join();
  client.join();

  REQUIRE(connect_result == 0);
  REQUIRE(conn.has_value());
  REQUIRE(conn->valid());
}

TEST_CASE("socket send", "[socket][io]") {
  lynx::net::socket_config cfg;
  lynx::net::socket_address addr{"127.0.0.1", 9304};

  // lynx::net::socket server(cfg);
  auto rs = lynx::net::socket::create(cfg);
  REQUIRE(rs.has_value());

  // server.bind(addr);
  // server.listen();
  REQUIRE(rs->bind(addr).has_value());
  REQUIRE(rs->listen().has_value());

  std::string_view msg = "Hello Lynx!";
  std::string received;

  int connecton_result = -1;

  std::thread client([&] {
    int raw = ::socket(AF_INET, SOCK_STREAM, 0);
    if (raw < 0)
      return;

    sockaddr_in native = addr.to_sockaddr_in();

    // ::connect(raw, reinterpret_cast<sockaddr *>(&native), sizeof(native));
    connecton_result =
        ::connect(raw, reinterpret_cast<sockaddr *>(&native), sizeof(native));

    char buf[128]{};

    ssize_t n = ::recv(raw, buf, sizeof(buf), 0);

    if (n > 0)
      received.assign(buf, n);

    ::close(raw);
  });

  // lynx::net::socket conn = server.accept();
  auto conn = rs->accept();
  REQUIRE(conn.has_value());

  auto bytes = std::as_bytes(std::span(msg));

  // ssize_t sent = conn.send(bytes);
  auto sent = conn->send(bytes);

  // REQUIRE(sent == msg.size());

  {
    lynx::net::socket done =
        std::move(*conn); // close our side so client's recv can finish
    // RAII (Resource Acquisition Is Initialization whatever that is
  }

  client.join();

  REQUIRE(connecton_result == 0); // The client successfully connected.
  REQUIRE(sent.has_value());      // The server successfully sent the message.
  REQUIRE(*sent ==
          msg.size()); // The number of bytes sent matches the message length.
  REQUIRE(received == msg); // The client received exactly the original message.
}

TEST_CASE("socket receive", "[socket][io]") {
  lynx::net::socket_config cfg;
  lynx::net::socket_address addr{"127.0.0.1", 9306};

  auto rs = lynx::net::socket::create(cfg);
  REQUIRE(rs.has_value());
  REQUIRE(rs->bind(addr).has_value());
  REQUIRE(rs->listen().has_value());

  std::string_view msg = "Ping Pong";

  int connection_result = -1;

  std::thread client([&] {
    int raw = ::socket(AF_INET, SOCK_STREAM, 0);

    if (raw < 0)
      return;

    sockaddr_in native = addr.to_sockaddr_in();

    connection_result =
        ::connect(raw, reinterpret_cast<sockaddr *>(&native), sizeof(native));

    ::send(raw, msg.data(), msg.size(), 0);
    ::close(raw);
  });

  auto conn = rs->accept();
  REQUIRE(conn.has_value());

  std::vector<std::byte> buffer(128);
  auto n = conn->receive(buffer);

  client.join();

  REQUIRE(connection_result == 0);
  REQUIRE(n.has_value());
  REQUIRE(*n == msg.size());
}

TEST_CASE("socket large payload", "[socket][large]") {
  lynx::net::socket_config cfg;
  lynx::net::socket_address addr{"127.0.0.1", 9305};

  // lynx::net::socket server(cfg);
  auto rs = lynx::net::socket::create(cfg);

  // server.bind(addr);
  // server.listen();
  REQUIRE(rs.has_value());
  REQUIRE(rs->bind(addr).has_value());
  REQUIRE(rs->listen().has_value());

  std::vector<std::byte> payload(1 << 20);
  size_t total = 0;
  int connection_status = -1;

  std::thread client([&] {
    int raw = ::socket(AF_INET, SOCK_STREAM, 0);
    if (raw < 0)
      return;

    sockaddr_in native = addr.to_sockaddr_in();

    connection_status =
        ::connect(raw, reinterpret_cast<sockaddr *>(&native), sizeof(native));

    std::vector<std::byte> buffer(payload.size());

    // size_t total = 0;

    while (total < buffer.size()) {
      ssize_t r = ::recv(raw, reinterpret_cast<char *>(buffer.data()) + total,
                         buffer.size() - total, 0);

      if (r <= 0)
        break;

      total += r;
    }

    // REQUIRE(total == payload.size());

    ::close(raw);
  });

  // lynx::net::socket conn = server.accept();
  auto conn = rs->accept();
  REQUIRE(conn.has_value());

  // ssize_t sent = conn.send(payload);
  auto sent = conn->send(payload);
  // REQUIRE(sent == payload.size());

  client.join();
  REQUIRE(connection_status == 0);
  REQUIRE(sent.has_value());
  REQUIRE(*sent == payload.size());
  REQUIRE(total == payload.size());
}