project(
	"ERM_Engine"
	VERSION 0.0.1
	DESCRIPTION "ERM Engine module"
	LANGUAGES CXX
)

erm_setup_module_library()

target_link_libraries(
	"${PROJECT_NAME}"
	PRIVATE 
		"${ERM_TARGET_API_LIB}"
		$<$<BOOL:${ERM_ASSIMP_ENABLED}>:assimp>
		$<$<BOOL:${ERM_OPEN_GL}>:libglew_static>
		glfw
		stb_image
		spirv-cross-cpp
		tinyxml2
		imgui
		fbx_sdk
		ERM_Math
		ERM_Audio
	PUBLIC
		ERM_Input
		ERM_Utils
)
