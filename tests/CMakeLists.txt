include(FetchContent)

FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.4.0
)

FetchContent_MakeAvailable(Catch2)

add_executable(unit_tests
    utils/test_string_utils.cpp
    http/test_http_parser.cpp
)

target_link_libraries(unit_tests PRIVATE core Catch2::Catch2WithMain)

target_include_directories(unit_tests
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

include(CTest)

include(${catch2_SOURCE_DIR}/extras/Catch.cmake)

catch_discover_tests(unit_tests)
