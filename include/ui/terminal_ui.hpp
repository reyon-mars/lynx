#pragma once
#include "net/socket_config.hpp"
#include "net/socket_type.hpp"

namespace ui
{
	class terminal_ui
	{
	private:
		static net::socket_config config_menu();
		static net::Type type_submenu();

	public:
		static void run(net::socket_config& cfg);
	};
} // namespace ui