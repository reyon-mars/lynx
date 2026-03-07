#include "http/http_parser.hpp"
#include "http/http_response.hpp"
#include "net/net_except.hpp"
#include "net/socket.hpp"
#include "utils/logger.hpp"
#include "utils/string_utils.hpp"
#include <array>
#include <cstddef>
#include <exception>
#include <span>
#include <string>
#include <string_view>
#include <sys/_types/_ssize_t.h>
#include <utility>
namespace net
{
	void handle_http_client(Socket&& client) noexcept
	{
		try
		{
			http::http_parser parser;
			std::array<std::byte, 4096> buffer;
			ssize_t bytes_received{0};

			bool keep_alive = true;

			while (keep_alive)
			{
				while ((bytes_received = client.receive(std::span(buffer))) > 0)
				{
					std::span<const std::byte> data(buffer.data(), static_cast<size_t>(bytes_received));
					auto result = parser.feed(data);

					if (result == http::http_parser::parse_result::complete)
					{
						break;
					}
					if (result == http::http_parser::parse_result::error)
					{
						utils::logger::log_err("HTTP Protocol Error.");
						return;
					}
				}

				auto request_opt = parser.extract();
				if (!request_opt)
				{
					utils::logger::log_err("Invalid request received.");
					return;
				}

				auto& request = *request_opt;
				http::http_response response;
				response.set_status(200);

				std::string body = "Method: " + request.method() + "\n" + "URI: " + request.uri() + "\n";

				response.set_header("Content-Type", "text/plain");
				response.set_header("Content-Length", std::to_string(body.size()));
				response.set_body(std::move(body));

				auto connection_header = request.get_header("Connection");
				if (connection_header && utils::equals_insensitive(utils::trim(*connection_header), "Close"))
				{
					keep_alive = false;
					response.set_header("Connection", "Close");
				}
				else
				{
					keep_alive = true;
				}

				auto bytes = response.serialize();
				client.send(std::as_bytes(std::span(bytes)));
			}
		}
		catch (const std::exception& e)
		{
			utils::logger::log_err(std::string("Exception in handle_http_client: ") + e.what());
		}
	}
} // namespace net