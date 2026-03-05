#pragma once
#include <array>
#include <cstddef>
#include <map>
#include <string>
#include <vector>

constexpr size_t REQ_LINE_SIZE = 3;

namespace http
{
	enum class req_idx : uint8_t
	{
		METHOD,
		URI,
		VERSION
	};

	class http_request
	{
	private:
		std::array<std::string, REQ_LINE_SIZE> req_line_;
		std::map<std::string, std::string> headers_;
		std::vector<std::byte> body_;

	public:
		void get_req_line();
	};
} // namespace http