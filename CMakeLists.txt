cmake_minimum_required(VERSION 3.21)
project(okx_price_viewer VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (MSVC)
    add_compile_options(/permissive- /Zc:__cplusplus)
endif()

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Network WebSockets)

qt_standard_project_setup()

qt_add_executable(okx_price_viewer
    source/main.cpp
    source/CMainWindow.cpp
    source/CMainWindow.hpp
    source/COrderBookWindow.cpp
    source/COrderBookWindow.hpp
)

target_link_libraries(
    okx_price_viewer
    PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Network
    Qt6::WebSockets
)
