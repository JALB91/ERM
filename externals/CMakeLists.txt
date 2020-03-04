# SETUP CMAKE_MODULE_PATH
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(GLFWConfig)
include(GLMConfig)

# ADD DEPENDENCIES BASED ON TARGET API
if ("${TARGET_API}" STREQUAL "OpenGl")
	include(GLEWConfig)
	# SETUP POLICIES
	add_subdirectory(glew-cmake)
	set(PUBLIC_DEPENDENCIES "libglew_static" PARENT_SCOPE)
endif()

# ADD EXTERNALS DEPENDENCIES SUBDIRECTORY
add_subdirectory(glm)
add_subdirectory(glfw)
add_subdirectory(imgui)
add_subdirectory(stb_image)

# SETUP POLICIES
add_subdirectory(tinyxml)