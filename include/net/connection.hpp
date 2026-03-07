#pragma once
#include "net/socket.hpp"

namespace net
{
	void echo(net::Socket&& client) noexcept;

	void handle_http_client( net::Socket&& client ) noexcept;
}