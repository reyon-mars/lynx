#pragma once
#include "net/socket_address.hpp"
#include "net/socket_type.hpp"
namespace net
{
	struct socket_config
	{
		Domain domain{Domain::IPv4};
		Type typ{Type::Sstream};
		Protocol proto{Protocol::Default};
		socket_address sock_addr{.ip = "0.0.0.0", .port = 8080};
	};
} // namespace net