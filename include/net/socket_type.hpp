#pragma once
#include <netinet/in.h>
#include <sys/socket.h>

namespace net
{
	using sockfd_t = int;

	enum class Domain
	{
		IPv4 = AF_INET,
		IPv6 = AF_INET6,
		UNIX = AF_UNIX,
		UNSPECIFIED = AF_UNSPEC
	};

	enum class Type
	{
		Sstream = SOCK_STREAM,
		Datagram = SOCK_DGRAM,
		Raw = SOCK_RAW
	};

	enum class Protocol
	{
		Default = 0,
		TCP = IPPROTO_TCP,
		UDP = IPPROTO_UDP
	};
} // namespace net