#pragma once
#include <cstddef>
#include <map>
#include <string>
#include <vector>
namespace http
{

	class http_response
	{
	private:
		std::string version_ = "HTTP/1.1";
		int status_code_ = 200;
		std::string reason_phrase_ = "OK";
		std::map<std::string, std::string> headers_;
		std::vector<std::byte> body_;

	public:
		void set_status_code(int status_code);
		void set_header(std::string name, std::string value);
		void set_body(std::string body);
		void set_body(std::vector<std::byte> body);

		void serialize_to(std::vector<std::byte>& output);
		std::vector<std::byte> serialize();
	};

} // namespace http