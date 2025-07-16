cmake_minimum_required(VERSION 3.11)

include(personal/personal.cmake)

if(DEFINED VCPKG_ROOT)
	set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
endif()

if(NOT CMAKE_TOOLCHAIN_FILE)
	message(FATAL_ERROR "You must set VCPKG_ROOT var in /personal/personal.cmake (or -DCMAKE_TOOLCHAIN_FILE). Check \"Build\" section in README.md.")
endif()

project(cutieproxy LANGUAGES CXX)

find_package(Microsoft.GSL REQUIRED)

if(DEFINED QT_ROOT)
	set(_QT_CMAKE_FILES ${QT_ROOT}/lib/cmake/Qt6)
	if(NOT IS_DIRECTORY ${QT_ROOT} OR NOT IS_DIRECTORY ${_QT_CMAKE_FILES})
		message(FATAL_ERROR "Couldn't find QT_ROOT or ${_QT_CMAKE_FILES} directory. Qt6 is required.")
	endif()

	set(CMAKE_PREFIX_PATH "${QT_ROOT};${_QT_CMAKE_FILES}")
	unset($_QT_CMAKE_FILES)

	find_program(QT_MOC_EXECUTABLE moc PATHS "${QT_ROOT}/bin")
	find_program(QT_UIC_EXECUTABLE uic PATHS "${QT_ROOT}/bin")
	find_program(QT_RCC_EXECUTABLE rcc PATHS "${QT_ROOT}/bin")
else()
	message(FATAL_ERROR "You must set QT_ROOT var in /personal/personal.cmake. Check \"Build\" section in README.md.")
endif()

add_subdirectory(ndisapi)
add_subdirectory(netlib)

include(cutie/cutie.cmake)
