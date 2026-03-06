#pragma once
#include "http/http_types.hpp"
#include <cstddef>
#include <string>
#include <string_view>
#include <vector>
namespace http
{
	class http_response
	{
	private:
		status_line status_line_;
		headers headers_;
		http_body body_;

	public:
		void set_status(int status_code);
		int status_code() const ;
		std::string_view reason_phrase() const;

		void set_header(std::string_view name, std::string value);
		void remove_header( std::string_view name );
		bool has_header( std::string_view name ) const ;

		void set_body(std::string_view body);
		void set_body(std::vector<std::byte> body);
		const http_body& body() const;

		void serialize_to(std::vector<std::byte>& output) const;
		std::vector<std::byte> serialize();

		bool is_error( ) const ;
	};

} // namespace http