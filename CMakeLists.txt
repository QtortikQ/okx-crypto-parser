cmake_minimum_required(VERSION 3.21)
project(okx_price_viewer VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (MSVC)
    add_compile_options(/permissive- /Zc:__cplusplus)
endif()

#set(Vulkan_INCLUDE_DIR "C:/VulkanSDK/1.4.321.1/Include")
#set(Vulkan_LIBRARY "C:/VulkanSDK/1.4.321.1/Lib/vulkan-1.lib")

#find_package(Vulkan REQUIRED)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Network WebSockets)

find_package(cpr CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

qt_standard_project_setup()

#set(QT_FEATURE_vulkan OFF)

qt_add_executable(okx_price_viewer
    source/main.cpp
    source/CMainWindow.cpp
    source/CMainWindow.hpp
    #source/CCryptoPair.hpp
    #source/ui/MainWindow.ui
    #source/ui/CryptoPairWindow.ui
)

target_link_libraries(
    okx_price_viewer
    PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Network
    Qt6::WebSockets
    #Vulkan::Vulkan
    cpr::cpr
    nlohmann_json::nlohmann_json
)
