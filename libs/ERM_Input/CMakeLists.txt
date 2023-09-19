erm_library_project(0.0.1 "ERM Input lib")

target_compile_definitions(
	"${PROJECT_NAME}"
	PRIVATE
		ERM_ZONE_COLOR=0x110000
)

target_link_libraries(
	"${PROJECT_NAME}"
	PUBLIC 
		glfw
		ERM_Math
)