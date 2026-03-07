cmake_minimum_required(VERSION 3.20)

project(lynx VERSION 1.0 LANGUAGES CXX)

# --------------------------------------------------
# Global Configuration
# --------------------------------------------------

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Qt auto processing (must be set before targets)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

option(BUILD_TUI "Build Terminal UI Version" ON)
option(BUILD_GUI "Build GUI version" OFF)

# --------------------------------------------------
# Core Library
# --------------------------------------------------

add_library(core STATIC
    src/net/connection.cpp
    src/net/socket.cpp
    src/net/socket_address.cpp
    src/utils/logger.cpp
    src/utils/thread_pool.cpp
    src/utils/string_utils.cpp
    src/http/http_request.cpp
    src/http/http_response.cpp
    src/http/http_parser.cpp
)

# Add project-level include directory
target_include_directories(core
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

if(UNIX)
    target_link_libraries(core PUBLIC pthread)
endif()

if(MSVC)
    target_compile_options(core PRIVATE /W4 /permissive-)
else()
    target_compile_options(core PRIVATE
        -Wall
        -Wextra
        -pedantic
        -Wshadow
        -Wconversion
    )
endif()

# --------------------------------------------------
# TUI Executable
# --------------------------------------------------

if(BUILD_TUI)

    add_executable(lynx_server
        src/ui/input_handler.cpp
        src/ui/menu_renderer.cpp
        src/ui/terminal_ui.cpp
        src/main_tui.cpp
    )

    # GUI and TUI both need project include
    target_include_directories(lynx_server
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    )

    target_link_libraries(lynx_server
        PRIVATE core
    )

    install(TARGETS lynx_server DESTINATION bin)

endif()

# --------------------------------------------------
# GUI Executable
# --------------------------------------------------

if(BUILD_GUI)

    find_package(Qt6 REQUIRED COMPONENTS Widgets)

    add_executable(lynx_gui
        # GUI source files
        src/gui/server_controller.cpp
        src/gui/config_widget.cpp
        src/gui/main_window.cpp
        src/main_gui.cpp

        # Explicitly list headers for AUTOMOC
        include/gui/server_controller.hpp
        include/gui/config_widget.hpp
        include/gui/main_window.hpp
    )

    # Ensure project-level includes are visible
    target_include_directories(lynx_gui
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    )

    # Link core and Qt
    target_link_libraries(lynx_gui
        PRIVATE
        core
        Qt6::Widgets
    )

    install(TARGETS lynx_gui DESTINATION bin)

endif()


enable_testing()
add_subdirectory(tests)
