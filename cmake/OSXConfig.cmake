function(erm_target_setup_project)
	set_target_properties("${PROJECT_NAME}" PROPERTIES
  		XCODE_SCHEME_WORKING_DIRECTORY "${CMAKE_INSTALL_PREFIX}"
	)

	target_compile_definitions(
		"${PROJECT_NAME}"
		PRIVATE 
			ERM_MAC
			$<$<BOOL:${ERM_OPEN_GL}>:GL_DO_NOT_WARN_IF_MULTI_GL_VERSION_HEADERS_INCLUDED>
			$<$<BOOL:${ERM_OPEN_GL}>:GL_SILENCE_DEPRECATION>
	)

	target_compile_options(
		"${PROJECT_NAME}"
		PRIVATE
			$<$<CONFIG:Debug>:-Wall -Werror -Wextra -pedantic-errors -Wno-gnu>
	)
endfunction()

