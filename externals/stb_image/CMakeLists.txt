project(stb_image)

file(GLOB_RECURSE SOURCES
	include/*.h
	src/*.cpp
)

add_library(stb_image ${SOURCES})

target_include_directories(stb_image PUBLIC include)
