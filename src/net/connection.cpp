#include "net/net_except.hpp"
#include "net/socket.hpp"
#include "utils/logger.hpp"
#include <array>
namespace net
{
	void echo(net::Socket&& client) noexcept
	{
		try
		{
			std::array<std::byte, 4096> buffer;
			ssize_t bytes_received{0};

			while ((bytes_received = client.receive(std::span(buffer))) > 0)
			{

				std::span<const std::byte> data(buffer.data(), static_cast<size_t>(bytes_received));
				utils::logger::log(std::string_view(reinterpret_cast<const char*>(data.data()), data.size()));

				client.send(data);
			}
			utils::logger::log("Client disconnected.");
		}
		catch (const net::net_except& e)
		{
			utils::logger::log_err(std::string("Network error in client thread : ") + e.what());
		}
		catch (const std::exception& e)
		{
			utils::logger::log_err(std::string("General error : ") + e.what());
		}
	}
} // namespace net