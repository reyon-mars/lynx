#include "ui/input_handler.hpp"
#include <iostream>

namespace ui
{
	uint16_t input_handler::get_uint16()
	{
		uint16_t user_in{0};

		while (true)
		{
			std::cout << ">> ";
			if (std::cin >> user_in)
			{
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				return user_in;
			}
			std::cout << "Invalid input. Enter a number. \n";
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		}
	}

	std::string input_handler::get_string()
	{
		std::string user_input;
		std::cout << ">> ";
		std::cin >> user_input;
		return user_input;
	}
} // namespace ui