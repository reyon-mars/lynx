#pragma once
#include <arpa/inet.h>
#include <cstdint>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
namespace net
{
	struct socket_address
	{
		std::string ip{"0.0.0.0"};
		uint16_t port{8080};

		sockaddr_in to_sockaddr_in() const;
	};

	bool is_valid_ipv4(const std::string& ip);
	bool is_valid_port(const int port_no);

} // namespace net