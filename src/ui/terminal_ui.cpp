#include "ui/terminal_ui.hpp"
#include "net/socket_address.hpp"
#include "net/socket_type.hpp"
#include "ui/color.hpp"
#include "ui/input_handler.hpp"
#include "ui/menu_renderer.hpp"
#include <iostream>

namespace ui
{
	net::socket_config terminal_ui::config_menu()
	{
		net::socket_config cfg{};
		cfg.sock_addr.ip = "0.0.0.0";
		cfg.sock_addr.port = 8080;
		cfg.typ = net::Type::Sstream;

		uint16_t user_choice{0};
		while (user_choice != 5)
		{

			menu_renderer::show_main_menu(cfg.sock_addr.ip,
										  cfg.sock_addr.port,
										  (cfg.typ == net::Type::Sstream ? "TCP" : "UDP"));

			user_choice = input_handler::get_uint16();

			switch (user_choice)
			{
			case 1:
			{
				std::cout << "Enter IP ";
				std::string ip = input_handler::get_string();
				if (net::is_valid_ipv4(ip))
				{
					cfg.sock_addr.ip = ip;
				}
				else
				{
					std::cout << "Invalid IP: " << ip << "\n";
				}
				break;
			}

			case 2:
			{
				std::cout << "Enter Port ";
				uint16_t port = input_handler::get_uint16();
				if (net::is_valid_port(port))
				{
					cfg.sock_addr.port = port;
				}
				else
				{
					std::cout << "Invalid port: " << port << "\n";
				}
				break;
			}
			case 3:
			{
				cfg.typ = type_submenu();
				break;
			}
			case 4:
			{
				cfg = net::socket_config{};
				cfg.typ = net::Type::Sstream;
				break;
			}
			case 5:
				break;
			default:
				std::cout << "Invalid selection, try again.\n";
				break;
			}
		}
		return cfg;
	}
	net::Type terminal_ui::type_submenu()
	{
		while (true)
		{
			menu_renderer::show_type_menu();
			uint16_t choice = input_handler::get_uint16();
			switch (choice)
			{
			case 1:
				return net::Type::Sstream;
			case 2:
				return net::Type::Datagram;
			default:
				std::cout << "Invalid selection, try again.\n";
				break;
			}
		}
	}

	void terminal_ui::run(net::socket_config& cfg)
	{
		std::cout << color::set_cyan("=== Mirror Stream Terminal ===\n");

		cfg = config_menu();

		std::cout << (color::set_green("\nConfiguration Complete\n"));

		std::cout << ("IP: " + cfg.sock_addr.ip + "\n");
		std::cout << ("Port: " + std::to_string(cfg.sock_addr.port) + "\n");
		std::cout << (std::string("Type: ") + (cfg.typ == net::Type::Sstream ? "TCP" : "UDP") + "\n");
	}
} // namespace ui