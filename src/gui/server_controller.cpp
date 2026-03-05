#include "gui/server_controller.hpp"
#include "net/connection.hpp"
#include "net/net_except.hpp"
#include "net/socket.hpp"
#include "net/socket_config.hpp"
#include <thread>

namespace gui
{
	server_controller::server_controller(QObject* parent)
	{
	}

	server_controller::~server_controller()
	{
		stop();
		if (worker_.joinable())
			worker_.join();
	}

	void server_controller::start(net::socket_config cfg)
	{
		if (running_)
			return;

		running_ = true;
		worker_ = std::thread(&server_controller::server_loop, this, cfg);
	}

	void server_controller::stop()
	{
		running_ = false;
	}

	void server_controller::server_loop(net::socket_config cfg)
	{
		try
		{
			net::Socket echo_server(net::socket_config{cfg.domain, cfg.typ, cfg.proto, cfg.sock_addr});
			echo_server.bind(cfg.sock_addr);
			echo_server.listen();

			emit log_msg(QString("Server listening on %1:%2")
							 .arg(QString::fromStdString(cfg.sock_addr.ip))
							 .arg(cfg.sock_addr.port));

			while (running_)
			{
				try
				{
					net::Socket client = echo_server.accept();
					emit log_msg("New client connected.");
					std::thread(net::echo, std::move(client)).detach();
				}
				catch (const net::net_except& e)
				{
					emit log_msg(QString("Accept error: %1").arg(e.what()));
				}
			}
		}
		catch (const net::net_except& e)
		{
			emit log_msg(QString("Server error: %1").arg(e.what()));
		}
		catch (const std::exception& e)
		{
			emit log_msg(QString("Unexpected error: %1").arg(e.what()));
		}
	}

} // namespace gui