erm_executable_project(1.0.0 "ERM Main lib")

erm_setup_resources_pipeline()

target_link_libraries(
	"${PROJECT_NAME}"
	PRIVATE
		ERM_Engine
)
