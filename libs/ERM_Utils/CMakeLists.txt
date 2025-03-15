erm_library_project(0.0.1 "ERM Utils lib")

target_compile_definitions(
	"${PROJECT_NAME}"
	PRIVATE
		ERM_ZONE_COLOR=0x111111
	PUBLIC
		ERM_DEBUG_STRING_IDS=$<CONFIG:Debug>
)

target_link_libraries(
	"${PROJECT_NAME}"
	PRIVATE
		xxHash
	PUBLIC
		$<$<BOOL:${ERM_TRACY_ENABLED}>:Tracy::TracyClient>
		magic_enum
		ERM_Math
		ERM_FileSystem
)
