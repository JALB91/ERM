cmake_minimum_required(VERSION 3.1)
project(erm)

set(CMAKE_CXX_STANDARD 14)

include_directories(
	include
)

file(GLOB SOURCES include/**.h src/**.cpp)

add_executable(erm ${SOURCES})
