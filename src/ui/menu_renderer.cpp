#include "ui/menu_renderer.hpp"
#include "ui/color.hpp"
#include <iomanip>
#include <iostream>

namespace ui
{
	void menu_renderer::show_main_menu(const std::string& ip, uint16_t port, const std::string& type)
	{
		const int label_width = 18;

		std::cout << color::bold << "\n--- Network Configuration ---\n" << color::reset;

		std::cout << std::left << std::setw(label_width) << "1. Set IP"
				  << "[" << color::cyan << ip << color::reset << "]\n";

		std::cout << std::left << std::setw(label_width) << "2. Set Port"
				  << "[" << color::cyan << port << color::reset << "]\n";

		std::cout << std::left << std::setw(label_width) << "3. Select Type"
				  << "[" << color::cyan << type << color::reset << "]\n";

		std::cout << "-----------------------------\n";
		std::cout << "4. Reset\n";
		std::cout << "5. " << color::bold << "Done" << color::reset << "\n";
	}

	void menu_renderer::show_type_menu()
	{
		const int opt_width = 4;
		std::cout << "\n" << color::bold << "Select Socket Type:" << color::reset << "\n";

		std::cout << std::left << std::setw(opt_width) << "1."
				  << "TCP " << color::cyan << "(Stream)" << color::reset << "\n";

		std::cout << std::left << std::setw(opt_width) << "2."
				  << "UDP " << color::cyan << "(Datagram)" << color::reset << "\n";

		std::cout << "-----------------------------\n";
	}
} // namespace ui