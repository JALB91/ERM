erm_library_project(0.0.1 "ERM Assets lib")

target_compile_definitions(
	"${PROJECT_NAME}"
	PRIVATE
		ERM_ZONE_COLOR=0x111100
)

target_link_libraries(
	"${PROJECT_NAME}"
	PRIVATE
		$<$<BOOL:${ERM_ASSIMP_ENABLED}>:assimp>
		stb_image
		tinyxml2
		fbx_sdk
		ERM_Utils
	PUBLIC
		ERM_Math
)
