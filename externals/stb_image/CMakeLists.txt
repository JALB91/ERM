project(stb_image)

file(GLOB_RECURSE SOURCES
	"${CMAKE_CURRENT_SOURCE_DIR}/include/*.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

add_library("${PROJECT_NAME}" 
	${SOURCES}
)

target_include_directories("${PROJECT_NAME}" 
	PUBLIC 
		"${CMAKE_CURRENT_SOURCE_DIR}/include"
)
