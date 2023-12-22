erm_executable_project(0.0.1 "ERM Setup lib")

target_compile_definitions(
	"${PROJECT_NAME}"
	PRIVATE
		ERM_ZONE_COLOR=0x000011
)

target_link_libraries(
	"${PROJECT_NAME}"
	PRIVATE
		ERM_FileSystem
	PUBLIC
		ERM_Utils
)
