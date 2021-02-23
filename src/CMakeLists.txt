cmake_minimum_required(VERSION 3.7...3.18)

if(${CMAKE_VERSION} VERSION_LESS 3.12)
	cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

project(RSSIDataLogger VERSION 0.0.1
		DESCRIPTION "Data logger for RSSI social distancing project thing."
		LANGUAGES CXX)

find_package(Qt5 REQUIRED COMPONENTS Core Widgets Gui SerialPort)

set(uiFiles
	MainWindow.ui)

set(headerFiles
	MainWindow.h)

set(cppFiles
	main.cpp
	MainWindow.cpp)


qt5_wrap_ui(wrappedUI ${uiFiles})
qt5_wrap_cpp(wrappedHeaders ${headerFiles})

add_executable(${PROJECT_NAME}
	${uiFiles}
	${headerFiles}
	${cppFiles}
	${wrappedUi}
	${wrappedHeaders})

target_link_libraries(${PROJECT_NAME}
	PUBLIC
	Qt5::Core
	Qt5::Widgets
	Qt5::Gui
	Qt5::SerialPort)