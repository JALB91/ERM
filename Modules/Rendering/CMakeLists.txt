project(
	"ERM_Rendering"
	VERSION 0.0.1
	DESCRIPTION "ERM Rendering module"
	LANGUAGES CXX
)

erm_setup_module_library()

target_compile_definitions(
	"${PROJECT_NAME}"
	PRIVATE
		ERM_ZONE_COLOR=0x111100
)

target_link_libraries(
	"${PROJECT_NAME}"
	PRIVATE
		imgui
		ERM_Window
	PUBLIC
		spirv-cross-cpp
		ERM_Input
		ERM_Assets
		ERM_Utils
)
