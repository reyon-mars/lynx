#include "net/socket_address.hpp"
#include "net/net_except.hpp"
#include <netinet/in.h>
#include <sys/_endian.h>
#include <sys/socket.h>

namespace net
{
	sockaddr_in socket_address::to_sockaddr_in() const
	{
		sockaddr_in addr{};
		addr.sin_family = AF_INET;
		addr.sin_port = htons(port);

		if (ip.empty() || ip == "0.0.0.0")
		{
			addr.sin_addr.s_addr = htonl(INADDR_ANY);
		}
		else
		{
			int result = ::inet_pton(AF_INET, ip.c_str(), &addr.sin_addr);
			if (result <= 0)
			{
				throw net_except("Invalid IP address format: " + ip);
			}
		}
		return addr;
	}

	bool is_valid_ipv4(const std::string& ip)

	{
		sockaddr_in addr{};
		return inet_pton(AF_INET, ip.c_str(), &addr.sin_addr.s_addr) == 1;
	}

	bool is_valid_port(const int port_no)
	{
		return port_no >= 1024 && port_no <= 65535;
	}
} // namespace net