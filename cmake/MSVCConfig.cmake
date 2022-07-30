function(erm_target_setup_project)
	set_property(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" PROPERTY VS_STARTUP_PROJECT "${PROJECT_NAME}")

	set_target_properties(
		"${PROJECT_NAME}" PROPERTIES
		VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_INSTALL_PREFIX}"
	)

	target_compile_definitions(
		"${PROJECT_NAME}"
		PRIVATE
			_CRT_SECURE_NO_WARNINGS
			_USE_MATH_DEFINES
			NOMINMAX
			ERM_WINDOWS
	)

	target_compile_options(
		"${PROJECT_NAME}"
		PRIVATE 
			$<$<CONFIG:Debug>:/W3 /WX>
		PUBLIC
			$<$<CONFIG:Release>:/ZI>
	)

	target_link_options(
		"${PROJECT_NAME}"
		PUBLIC
			$<$<CONFIG:Debug>:/INCREMENTAL:NO /DEBUG /OPT:REF /OPT:ICF>
	)
endfunction()
