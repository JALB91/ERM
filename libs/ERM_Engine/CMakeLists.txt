erm_library_project(0.0.1 "ERM Engine lib")

target_compile_definitions(
	"${PROJECT_NAME}"
	PRIVATE
		ERM_ZONE_COLOR=0x000000
)

target_link_libraries(
	"${PROJECT_NAME}"
	PRIVATE 
		"${ERM_RENDERING_API_LIB}"
		glfw
		ERM_Math
		ERM_Audio
		ERM_Assets
		ERM_ECS
	PUBLIC
		ERM_Input
		ERM_Utils
		ERM_Rendering
		ERM_Window
)
