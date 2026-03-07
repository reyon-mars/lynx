#pragma once
#include "net/socket_address.hpp"
#include "net/socket_config.hpp"
#include "net/socket_type.hpp"
#include <arpa/inet.h>
#include <cstddef>
#include <span>
#include <sys/_types/_ssize_t.h>
#include <sys/socket.h>

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