project(
	"ERM_Main"
	VERSION 1.0.0
	DESCRIPTION "ERM Main module"
	LANGUAGES CXX
)

erm_setup_module_executable()

target_link_libraries(
	"${PROJECT_NAME}"
	PRIVATE
		ERM_Engine
)
