cmake_minimum_required(VERSION 3.11)
project(cutie LANGUAGES CXX)

find_package(Qt6 COMPONENTS Core Gui Widgets REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(PLATFORM "x64")
set(CMAKE_CXX_STANDARD 23)

file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_LIST_DIR}/*.cpp")

add_executable(cutie ${SOURCES})

target_link_libraries(cutie PRIVATE 
	netlib
	Qt6::Core
	Qt6::Gui
	Qt6::Widgets
)