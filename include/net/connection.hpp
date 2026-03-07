#pragma once
#include "http/router.hpp"
#include "net/socket.hpp"

namespace net
{
	void handle_http_client(net::Socket&& client, http::router& router_) noexcept;
}