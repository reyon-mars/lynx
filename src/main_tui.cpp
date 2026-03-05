#include "net/connection.hpp"
#include "net/connection_stats.hpp"
#include "net/net_except.hpp"
#include "net/socket.hpp"
#include "ui/term.hpp"
#include "ui/terminal_ui.hpp"
#include "utils/logger.hpp"
#include "utils/thread_pool.hpp"
#include <iostream>
#include <string>

int main()
{
	auto& term_ctx = ui::term::terminal_context::get_instance();
	term_ctx.clear();

	try
	{
		utils::thread_pool t_pool;

		net::socket_config config{};
		ui::terminal_ui::run(config);

		net::Socket echo_server(config);

		echo_server.bind(config.sock_addr);
		echo_server.listen();

		std::cout << "Echo server listening on " << config.sock_addr.ip << " : " << config.sock_addr.port << "\n";

		while (true)
		{
			try
			{
				net::Socket client = echo_server.accept();
				utils::logger::log("New client connected.");

				auto task = [client = std::move(client), stats = net::connection_stats{}]() mutable
				{
					{
						net::echo(std::move(client));
					}
					utils::logger::log("Active Connections: " +
									   std::to_string(net::connection_stats::get_active_conn()));
				};
				t_pool.submit(std::move(task));

				utils::logger::log("Active Connections: " + std::to_string(net::connection_stats::get_active_conn()));
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