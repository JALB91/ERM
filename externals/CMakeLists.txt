project(
	"ERM_Externals"
	VERSION 1.0.0
	DESCRIPTION "ERM External libs"
)

# Include configuration files for dependencies
include(TracyConfig)
include(GLMConfig)
include(GLFWConfig)
include(TinyXmlConfig)
include(SPIRVConfig)
include(AssimpConfig)

# Adds common externals dependencies
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/tracy")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/glm")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/glfw")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/imgui")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/tinyxml")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/stb_image")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/SPIRV-Cross")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/fmod")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/fbx")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/json")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/magic_enum")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/refl-cpp")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/xxHash")

if(ERM_ASSIMP_ENABLED)
	add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/assimp")
endif()
