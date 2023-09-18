erm_executable_project(1.0.0 "ERM Main lib")
erm_setup_resources_pipeline()

target_link_libraries(
	"${PROJECT_NAME}"
	PRIVATE
		ERM_Engine
)

if(ERM_WINDOWS)
	# Copy DLLs
	add_custom_command(TARGET "${PROJECT_NAME}" POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:${PROJECT_NAME}> $<TARGET_FILE_DIR:${PROJECT_NAME}>
		COMMAND_EXPAND_LISTS
	)
endif()
