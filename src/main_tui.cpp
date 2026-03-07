#include "net/connection.hpp"
#include "net/connection_stats.hpp"
#include "net/net_except.hpp"
#include "net/socket.hpp"
#include "net/socket_config.hpp"
#include "ui/term_context.hpp"
#include "ui/terminal_ui.hpp"
#include "utils/logger.hpp"
#include "utils/thread_pool.hpp"
#include <exception>
#include <iostream>
#include <string>
#include <utility>

int main()
{
	auto& term_ctx = ui::term::terminal_context::get_instance();
	term_ctx.clear();

	try
	{
		utils::thread_pool t_pool;

		net::socket_config config{};
		ui::terminal_ui::run(config);

		net::Socket http_server(config);
		http_server.bind(config.sock_addr);
		http_server.listen();

		std::cout << "HTTP Server listening on " << config.sock_addr.ip << ":" << config.sock_addr.port << "\n";

		while (true)
		{
			try
			{
				net::Socket client = http_server.accept();
				utils::logger::log("New client connected.");

				auto task = [client_socket = std::move(client)]() mutable
				{
					net::connection_stats stats;
					utils::logger::log("Total Active Connections: " + std::to_string(stats.get_active_conn()));

					net::handle_http_client(std::move(client_socket));

					utils::logger::log("Connection closed. Active: " + std::to_string(stats.get_active_conn() - 1));
					utils::logger::log("Total Connections Served: " + std::to_string(stats.get_total_conn()) );
				};

				t_pool.submit(std::move(task));
			}
			catch (const net::net_except& e)
			{
				utils::logger::log_err(std::string("Accept error: ") + e.what());
			}
		}
	}
	catch (const net::net_except& e)
	{
		utils::logger::log_err(std::string("Server error: ") + e.what());
		return 1;
	}
	catch (const std::exception& e)
	{
		utils::logger::log_err(std::string("Unexpected error: ") + e.what());
		return 2;
	}

	return 0;
}