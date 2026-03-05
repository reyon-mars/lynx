#pragma once
#include <cstdint>
#include <string>

namespace ui
{
	class input_handler
	{

	public:
		static uint16_t get_uint16();
		static std::string get_string();
	};
} // namespace ui