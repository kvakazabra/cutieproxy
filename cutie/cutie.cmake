cmake_minimum_required(VERSION 3.11)
project(cutie)

find_package(Qt6 COMPONENTS Core Gui Widgets REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(PLATFORM "x64")
set(CMAKE_CXX_STANDARD 23)

set(SOURCES
	${CMAKE_CURRENT_LIST_DIR}/main.cpp
	${CMAKE_CURRENT_LIST_DIR}/ui/CpWindow.cpp
	${CMAKE_CURRENT_LIST_DIR}/socksify/Socksifier.cpp
)

set(UI 
	${CMAKE_CURRENT_LIST_DIR}/ui/CpWindow.ui
)

add_executable(cutie ${SOURCES} ${UI})

target_link_libraries(cutie PRIVATE 
	netlib
	ndisapi
	Microsoft.GSL::GSL
	Qt6::Core
	Qt6::Gui
	Qt6::Widgets
)

set(PLATFORM "x64")

set_target_properties(cutie PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin-${PLATFORM}"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin-${PLATFORM}"
)