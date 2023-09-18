erm_library_project(0.0.1 "ERM Audio lib")

target_compile_definitions(
	"${PROJECT_NAME}"
	PRIVATE
		ERM_ZONE_COLOR=0x001100
)

target_link_libraries(
	"${PROJECT_NAME}"
	PUBLIC
		fmod_core
		fmod_studio
		ERM_Utils
)