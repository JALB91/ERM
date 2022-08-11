project(
	"ERM_Utils"
	VERSION 0.0.1
	DESCRIPTION "ERM Utils module"
	LANGUAGES CXX
)

erm_setup_module_library()

target_compile_definitions(
	"${PROJECT_NAME}"
	PRIVATE
		ERM_ZONE_COLOR=0x111111
)

target_link_libraries(
	"${PROJECT_NAME}"
	PUBLIC
		$<$<BOOL:${ERM_TRACY_ENABLED}>:Tracy::TracyClient>
)
