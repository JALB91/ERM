cmake_minimum_required(VERSION 3.1)

set(PROJECT_NAME ERM)

project(${PROJECT_NAME})

set(GLM_TEST_ENABLE_CXX_98 OFF CACHE BOOL "" FORCE)
set(GLM_TEST_ENABLE_CXX_11 OFF CACHE BOOL "" FORCE)
set(GLM_TEST_ENABLE_CXX_14 OFF CACHE BOOL "" FORCE)
set(GLM_TEST_ENABLE_CXX_17 OFF CACHE BOOL "" FORCE)
set(GLM_TEST_ENABLE_CXX_20 OFF CACHE BOOL "" FORCE)
set(GLM_TEST_ENABLE OFF CACHE BOOL "" FORCE)
set(GLM_TEST_ENABLE_SIMD_SSE2 OFF CACHE BOOL "" FORCE)
set(GLM_TEST_ENABLE_SIMD_SSE3 OFF CACHE BOOL "" FORCE)
set(GLM_TEST_ENABLE_SIMD_SSSE3 OFF CACHE BOOL "" FORCE)
set(GLM_TEST_ENABLE_SIMD_SSE4_1 OFF CACHE BOOL "" FORCE)
set(GLM_TEST_ENABLE_SIMD_SSE4_2 OFF CACHE BOOL "" FORCE)
set(GLM_TEST_ENABLE_SIMD_AVX OFF CACHE BOOL "" FORCE)
set(GLM_TEST_ENABLE_SIMD_AVX2 OFF CACHE BOOL "" FORCE)
set(GLM_TEST_FORCE_PURE OFF CACHE BOOL "" FORCE)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

set(CMAKE_CXX_STANDARD 14)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS}")

add_compile_definitions(
	GL_SILENCE_DEPRECATION 
	GL_DO_NOT_WARN_IF_MULTI_GL_VERSION_HEADERS_INCLUDED 
	IMGUI_IMPL_OPENGL_LOADER_GLEW
)

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
add_subdirectory(externals/glm)
add_subdirectory(externals/glfw)
add_subdirectory(externals/imgui)
add_subdirectory(externals/stb_image)

add_compile_definitions(GLFW_INCLUDE_GLCOREARB)

include_directories(
	${GLEW_INCLUDE_DIR}
	include
)

file(GLOB_RECURSE SOURCES 
	include/**.h 
	src/**.cpp
)

if (${PRINT_VARIABLES})
	get_cmake_property(_variableNames VARIABLES)
	list (SORT _variableNames)
	foreach (_variableName ${_variableNames})
    	message(STATUS "${_variableName}=${${_variableName}}")
	endforeach()

	execute_process(COMMAND "${CMAKE_COMMAND}" "-E" "environment")
endif()

add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(
	${PROJECT_NAME}
	${OPENGL_gl_LIBRARY}
	${GLEW_LIBRARIES}
	glfw
	glm
	imgui
	stb_image
)

set(res_source ${CMAKE_HOME_DIRECTORY}/res/)
set(res_dest ${CMAKE_CURRENT_BINARY_DIR}/Debug/res)

add_custom_command(
 TARGET ${PROJECT_NAME} POST_BUILD
 COMMAND ${CMAKE_COMMAND} -E copy_directory ${res_source} ${res_dest}
 DEPENDS ${res_dest}
 COMMENT "symbolic link resources folder from ${res_source} => ${res_dest}"
)
