project(
	"ERM_Input"
	VERSION 0.0.1
	DESCRIPTION "ERM Input lib"
	LANGUAGES CXX
)

erm_setup_library()

target_compile_definitions(
	"${PROJECT_NAME}"
	PRIVATE
		ERM_ZONE_COLOR=0x110000
)

target_link_libraries(
	"${PROJECT_NAME}"
	PUBLIC 
		glfw
)