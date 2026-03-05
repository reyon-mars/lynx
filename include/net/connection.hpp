#pragma once
#include "net/socket.hpp"

namespace net
{
	void echo(net::Socket&& client) noexcept;
}