project(
	"ERM_Editor"
	VERSION 0.0.1
	DESCRIPTION "ERM Editor"
	LANGUAGES CXX
)

erm_setup_executable()

target_compile_definitions(
	"${PROJECT_NAME}"
	PRIVATE
		ERM_ZONE_COLOR=0x111000
)

target_link_libraries(
	"${PROJECT_NAME}"
	PRIVATE
		imgui
	PUBLIC
		ERM_Engine
		ERM_Utils
		ERM_Math
		ERM_Rendering
		ERM_ECS
		ERM_FileSystem
		ERM_Window
		ERM_Audio
		ERM_Assets
		ERM_Input
		glfw
)