erm_lib_setup(
	VERSION 0.0.1
	DESCRIPTION "ERM Utils lib"
	LANGUAGES CXX
	STATIC
	PRIVATE_DEPS
		xxHash
	PUBLIC_DEPS
		$<$<BOOL:${ERM_TRACY_ENABLED}>:Tracy::TracyClient>
		magic_enum
		ERM_Math_static
		ERM_FileSystem_static
		ERM_Log_static
)

target_compile_definitions(
	${PROJECT_NAME}_static
	PRIVATE
		ERM_ZONE_COLOR=0x111111
	PUBLIC
		ERM_DEBUG_STRING_IDS=$<CONFIG:Debug>
)
