cmake_minimum_required(VERSION 3.11)
project(cutieproxy LANGUAGES CXX)

include(personal/personal.cmake OPTIONAL)

if(DEFINED QT_ROOT)
	set(_QT_CMAKE_FILES ${QT_ROOT}/lib/cmake/Qt6)
	if(NOT IS_DIRECTORY ${QT_ROOT} OR NOT IS_DIRECTORY ${_QT_CMAKE_FILES})
		message(FATAL_ERROR "Couldn't find QT_ROOT or ${_QT_CMAKE_FILES} directory. Qt6 is required.")
	endif()

	set(CMAKE_PREFIX_PATH "${QT_ROOT};${_QT_CMAKE_FILES}")
	unset($_QT_CMAKE_FILES)
endif()

add_subdirectory(ndisapi)
add_subdirectory(netlib)

include(cutie/cutie.cmake)
