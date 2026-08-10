#include "http/mime.hpp"
#include <catch2/catch_test_macros.hpp>


TEST_CASE("get_mime_type returns correct MIME", "[mime]")
{
    REQUIRE( http::get_mime_type(".html") == "text/html" );
    

}