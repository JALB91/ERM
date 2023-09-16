project(
	"ERM_Main"
	VERSION 1.0.0
	DESCRIPTION "ERM Main lib"
	LANGUAGES CXX
)

erm_setup_executable()
erm_setup_resources_pipeline()

target_link_libraries(
	"${PROJECT_NAME}"
	PRIVATE
		ERM_Engine
)
