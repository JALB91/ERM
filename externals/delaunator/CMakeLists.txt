project(delaunator)

include_directories(
	include
)

file(GLOB SOURCES
	include/*.h
	src/*.cpp
)

add_library(delaunator ${SOURCES})

target_include_directories(delaunator PUBLIC include)
