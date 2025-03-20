function(erm_target_setup_project)
	set_target_properties(
		"${PROJECT_NAME}" 
		PROPERTIES
  			XCODE_SCHEME_WORKING_DIRECTORY "${CMAKE_INSTALL_PREFIX}"
	)

	get_target_property(IS_INTERFACE "${PROJECT_NAME}" IS_INTERFACE)
	
	if(IS_INTERFACE)
		target_compile_definitions(
			"${PROJECT_NAME}"
			INTERFACE 
				ERM_OSX
				$<$<BOOL:${ERM_OPEN_GL}>:GL_DO_NOT_WARN_IF_MULTI_GL_VERSION_HEADERS_INCLUDED>
				$<$<BOOL:${ERM_OPEN_GL}>:GL_SILENCE_DEPRECATION>
				VULKAN_HPP_NO_SPACESHIP_OPERATOR
		)

		target_compile_options(
			"${PROJECT_NAME}"
			INTERFACE
				$<$<CONFIG:Debug>:-Wall -Werror -Wextra -pedantic-errors -Wno-gnu>
				-fno-exceptions -fno-rtti
		)
	else()
		target_compile_definitions(
			"${PROJECT_NAME}"
			PRIVATE 
				ERM_OSX
				$<$<BOOL:${ERM_OPEN_GL}>:GL_DO_NOT_WARN_IF_MULTI_GL_VERSION_HEADERS_INCLUDED>
				$<$<BOOL:${ERM_OPEN_GL}>:GL_SILENCE_DEPRECATION>
				VULKAN_HPP_NO_SPACESHIP_OPERATOR
		)

		target_compile_options(
			"${PROJECT_NAME}"
			PRIVATE
				$<$<CONFIG:Debug>:-Wall -Werror -Wextra -pedantic-errors -Wno-gnu>
				-fno-exceptions -fno-rtti
		)
	endif()
endfunction()

