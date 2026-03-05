#pragma once
#include <string>
#include <string_view>

namespace ui::color
{

	constexpr std::string_view reset = "\033[0m";
	constexpr std::string_view bold = "\033[1m";
	constexpr std::string_view dim = "\033[2m";
	constexpr std::string_view italic = "\033[3m";
	constexpr std::string_view underline = "\033[4m";
	constexpr std::string_view blink = "\033[5m";
	constexpr std::string_view invert = "\033[7m";
	constexpr std::string_view hidden = "\033[8m";

	constexpr std::string_view black = "\033[30m";
	constexpr std::string_view red = "\033[31m";
	constexpr std::string_view green = "\033[32m";
	constexpr std::string_view yellow = "\033[33m";
	constexpr std::string_view blue = "\033[34m";
	constexpr std::string_view magenta = "\033[35m";
	constexpr std::string_view cyan = "\033[36m";
	constexpr std::string_view white = "\033[37m";

	constexpr std::string_view b_black = "\033[90m";
	constexpr std::string_view b_red = "\033[91m";
	constexpr std::string_view b_green = "\033[92m";
	constexpr std::string_view b_yellow = "\033[93m";
	constexpr std::string_view b_blue = "\033[94m";
	constexpr std::string_view b_magenta = "\033[95m";
	constexpr std::string_view b_cyan = "\033[96m";
	constexpr std::string_view b_white = "\033[97m";

	constexpr std::string_view bg_black = "\033[40m";
	constexpr std::string_view bg_red = "\033[41m";
	constexpr std::string_view bg_green = "\033[42m";
	constexpr std::string_view bg_yellow = "\033[43m";
	constexpr std::string_view bg_blue = "\033[44m";
	constexpr std::string_view bg_magenta = "\033[45m";
	constexpr std::string_view bg_cyan = "\033[46m";
	constexpr std::string_view bg_white = "\033[47m";

	inline std::string apply(std::string_view code, const std::string& text)
	{
		return std::string(code) + text + std::string(reset);
	}

	inline std::string set_bold(const std::string& s)
	{
		return apply(bold, s);
	}
	inline std::string set_dim(const std::string& s)
	{
		return apply(dim, s);
	}
	inline std::string set_italic(const std::string& s)
	{
		return apply(italic, s);
	}
	inline std::string set_underline(const std::string& s)
	{
		return apply(underline, s);
	}
	inline std::string set_blink(const std::string& s)
	{
		return apply(blink, s);
	}
	inline std::string set_invert(const std::string& s)
	{
		return apply(invert, s);
	}
	inline std::string set_hidden(const std::string& s)
	{
		return apply(hidden, s);
	}

	inline std::string set_black(const std::string& s)
	{
		return apply(black, s);
	}
	inline std::string set_red(const std::string& s)
	{
		return apply(red, s);
	}
	inline std::string set_green(const std::string& s)
	{
		return apply(green, s);
	}
	inline std::string set_yellow(const std::string& s)
	{
		return apply(yellow, s);
	}
	inline std::string set_blue(const std::string& s)
	{
		return apply(blue, s);
	}
	inline std::string set_magenta(const std::string& s)
	{
		return apply(magenta, s);
	}
	inline std::string set_cyan(const std::string& s)
	{
		return apply(cyan, s);
	}
	inline std::string set_white(const std::string& s)
	{
		return apply(white, s);
	}

	inline std::string set_b_black(const std::string& s)
	{
		return apply(b_black, s);
	}
	inline std::string set_b_red(const std::string& s)
	{
		return apply(b_red, s);
	}
	inline std::string set_b_green(const std::string& s)
	{
		return apply(b_green, s);
	}
	inline std::string set_b_yellow(const std::string& s)
	{
		return apply(b_yellow, s);
	}
	inline std::string set_b_blue(const std::string& s)
	{
		return apply(b_blue, s);
	}
	inline std::string set_b_magenta(const std::string& s)
	{
		return apply(b_magenta, s);
	}
	inline std::string set_b_cyan(const std::string& s)
	{
		return apply(b_cyan, s);
	}
	inline std::string set_b_white(const std::string& s)
	{
		return apply(b_white, s);
	}

	inline std::string set_bg_black(const std::string& s)
	{
		return apply(bg_black, s);
	}
	inline std::string set_bg_red(const std::string& s)
	{
		return apply(bg_red, s);
	}
	inline std::string set_bg_green(const std::string& s)
	{
		return apply(bg_green, s);
	}
	inline std::string set_bg_yellow(const std::string& s)
	{
		return apply(bg_yellow, s);
	}
	inline std::string set_bg_blue(const std::string& s)
	{
		return apply(bg_blue, s);
	}
	inline std::string set_bg_magenta(const std::string& s)
	{
		return apply(bg_magenta, s);
	}
	inline std::string set_bg_cyan(const std::string& s)
	{
		return apply(bg_cyan, s);
	}
	inline std::string set_bg_white(const std::string& s)
	{
		return apply(bg_white, s);
	}
} // namespace ui::color