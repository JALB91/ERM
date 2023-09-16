project(
	"ERM_Audio"
	VERSION 0.0.1
	DESCRIPTION "ERM Audio lib"
	LANGUAGES CXX
)

erm_setup_library()

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