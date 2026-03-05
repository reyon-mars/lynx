#include "utils/logger.hpp"
#include "ui/color.hpp"
#include <chrono>
#include <format>
#include <iostream>
#include <mutex>
#include <string_view>

namespace utils
{
	std::mutex logger::log_mtx_;

	void logger::emit_log(std::string_view log_level, std::string_view log_msg, std::string_view log_color)
	{
		auto now = std::chrono::floor<std::chrono::seconds>(std::chrono::system_clock::now());
		{
			std::lock_guard<std::mutex> lock(log_mtx_);
			std::cout
				<< std::format("[{:%F %T}] {} {:<5}{} {}\n", now, log_color, log_level, ui::color::reset, log_msg);
		}
	}

	void logger::log(std::string_view msg)
	{
		emit_log("LOG", msg, ui::color::blue);
	}

	void logger::log_err(std::string_view err_msg)
	{
		emit_log("ERROR", err_msg, ui::color::red);
	}
	void logger::log_warn(std::string_view warn_msg)
	{
		emit_log("WARNING", warn_msg, ui::color::blue);
	}
} // namespace utils