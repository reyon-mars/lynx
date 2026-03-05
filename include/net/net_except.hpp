#pragma once
#include <cerrno>
#include <cstring>
#include <stdexcept>
#include <string>

namespace net
{
	class net_except : public std::runtime_error
	{
	private:
		static std::string build_message(const std::string& msg)
		{
			return msg + " | error : " + std::to_string(errno) + " ( " + std::strerror(errno) + " ). \n";
		}

	public:
		explicit net_except(const std::string& msg) : std::runtime_error(build_message(msg))
		{
		}

		explicit net_except(const char* msg) : std::runtime_error(msg)
		{
		}
	};
} // namespace net