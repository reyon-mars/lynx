#pragma once
#include <iostream>

namespace ui::term
{
	class terminal_context
	{
	private:
		terminal_context()
		{
			enter_fullscreen();
		}
		~terminal_context()
		{
			exit_fullscreen();
		}
		inline void enter_fullscreen()
		{
			std::cout << "\033[?1049h" << "\033[H";
		}

		inline void exit_fullscreen()
		{
			std::cout << "\033[?1049l";
		}

	public:
		terminal_context(const terminal_context& other) = delete;
		terminal_context& operator=(const terminal_context& other) = delete;
		terminal_context& operator=(const terminal_context&& other) = delete;

		static terminal_context& get_instance()
		{
			static terminal_context instance;
			return instance;
		}
		inline void clear()
		{
			std::cout << "\033[2J\033[H";
		}
	};

} // namespace ui::term