project(
	"ERM_ECS"
	VERSION 0.0.1
	DESCRIPTION "ERM Entity Component System"
	LANGUAGES CXX
)

erm_setup_library()

target_compile_definitions(
	"${PROJECT_NAME}"
	PRIVATE
		ERM_ZONE_COLOR=0x110100
)

target_link_libraries(
	"${PROJECT_NAME}"
	PRIVATE
		ERM_Input
		ERM_Utils
		ERM_Assets
		ERM_Rendering
	PUBLIC
		ERM_Math
)
