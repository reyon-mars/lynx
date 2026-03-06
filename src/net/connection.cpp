#include "http/http_parser.hpp"
#include "http/http_response.hpp"
#include "net/net_except.hpp"
#include "net/socket.hpp"
#include "utils/logger.hpp"
#include <array>
#include <string>
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

	void handle_http_client(Socket&& client)
	{
		http::http_parser parser;

		while (true)
		{
			std::span<std::byte> buffer;
			client.receive(buffer);

			auto result = parser.feed(buffer);
			if (result == http::http_parser::parse_result::complete || result == http::http_parser::parse_result::error)
			{
				break;
			}
		}

		auto request_opt = parser.extract();
		if (!request_opt)
		{
			utils::logger::log_err("Invalid request received.");
		}
		auto request = request_opt.value();
		http::http_response response;
		response.set_status(200);
		std::string body = "Method: " + request.method() + "\n" + "URI: " + request.uri() + "\n";
		response.set_body( std::move(body));
		response.set_header( "Content-type", "text/plain" );
		response.set_header("Content-Length", std::to_string(body.size()));

		auto bytes = response.serialize();

		client.send(bytes);
	}
} // namespace net