#pragma once
#include <cstdint>
#include <string>

namespace ui
{
	class menu_renderer
	{
	public:
		static void show_main_menu(const std::string& ip, uint16_t port, const std::string& type);
		static void show_type_menu();
	};
} // namespace ui