#pragma once
#include <mutex>
#include <string_view>

namespace utils
{
	class logger
	{
	private:
		static std::mutex log_mtx_;

		static void emit_log(std::string_view log_level, std::string_view log_msg, std::string_view log_color);

	public:
		static void log(std::string_view msg);
		static void log_err(std::string_view err_msg);
		static void log_warn(std::string_view warn_msg);
	};
} // namespace utils