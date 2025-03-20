erm_library_project(0.0.1 "ERM Rendering lib")

if(ERM_RENDERING_API_INCLUDE_DIR)
	target_sources(
		"${PROJECT_NAME}"
		PUBLIC
			FILE_SET HEADERS
			BASE_DIRS "${ERM_RENDERING_API_INCLUDE_DIR}"
	)
endif()

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
		${ERM_RENDERING_API_LIB}
		spirv-cross-cpp
		ERM_Input
		ERM_Utils
		ERM_Assets
)
