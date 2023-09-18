project(
	"ERM_Engine"
	VERSION 0.0.1
	DESCRIPTION "ERM Engine lib"
	LANGUAGES CXX
)

erm_setup_library()

target_compile_definitions(
	"${PROJECT_NAME}"
	PRIVATE
		ERM_ZONE_COLOR=0x000000
)

target_link_libraries(
	"${PROJECT_NAME}"
	PRIVATE 
		"${ERM_TARGET_API_LIB}"
		glfw
		ERM_Math
		ERM_Audio
		ERM_Assets
		ERM_ECS
		# ERM_Editor
	PUBLIC
		ERM_Input
		ERM_Utils
		ERM_Rendering
		ERM_Window
)
