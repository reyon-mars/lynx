#pragma once

// Project specific includes
#include "net/socket_address.hpp"
#include "net/socket_config.hpp"
#include "net/socket_type.hpp"

#include <cstddef>
#include <span>

// Platform specific includes

#if defined(_WIN32) || defined(__WIN64__) || defined(WIN32) || defined(__WIN32__ )
	#ifndef WIN32_LEAN_AND_MEAN
		#define WIN32_LEAN_AND_MEAN
	#endif
	#include <winsock2.h>
	#include <ws2tcpip.h>

	using sockfd_t = SOCKET;
	using ssize_t = std::size_t;

	#define INVALID_SOCKET_VAL INVALID_SOCKET
#else
	#include <sys/socket.h>
	#include <arpa/inet.h>
	#include <unistd.h>
	#include <sys/types.h>
	
	using sockfd_t = int;
	#define INVALID_SOCKET_VAL (-1)
#endif

namespace net
{
	class Socket
	{
	private:
		socket_config sock_cfg{};
		sockfd_t sock_fd{-1};

		Socket(int fd, const socket_config& cfg);

	public:
		~Socket();

		Socket(const socket_config& cfg);

		Socket(const Socket&) = delete;
		Socket& operator=(const Socket&) = delete;

		Socket(Socket&& other) noexcept;
		Socket& operator=(Socket&& other) noexcept;

		void bind(const socket_address& sock_addr);
		void listen(int backlog = SOMAXCONN);
		[[nodiscard]] Socket accept();

		ssize_t send(std::span<const std::byte> data);
		ssize_t receive(std::span<std::byte> buffer);
	};
} // namespace net