project(
	"ERM_Math"
	VERSION 0.0.1
	DESCRIPTION "ERM Math module"
	LANGUAGES CXX
)

erm_setup_module_library()

target_compile_definitions(
	"${PROJECT_NAME}"
	PRIVATE
		ERM_ZONE_COLOR=0x000011
)

target_link_libraries(
	"${PROJECT_NAME}"
	PUBLIC 
        glm
)