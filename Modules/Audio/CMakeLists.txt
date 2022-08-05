project(
	"ERM_Audio"
	VERSION 0.0.1
	DESCRIPTION "ERM Audio module"
	LANGUAGES CXX
)

erm_setup_module_library()

target_link_libraries(
	"${PROJECT_NAME}"
	PUBLIC
		fmod_core
		fmod_studio
		ERM_Utils
)