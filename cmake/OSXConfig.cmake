function(target_setup_project)
	set_target_properties("${PROJECT_NAME}" PROPERTIES
  		XCODE_SCHEME_WORKING_DIRECTORY "${CMAKE_INSTALL_PREFIX}"
	)

	target_compile_definitions(
		"${PROJECT_NAME}"
		PRIVATE 
			ERM_OSX
			$<$<BOOL:${ERM_OPEN_GL}>:GL_DO_NOT_WARN_IF_MULTI_GL_VERSION_HEADERS_INCLUDED>
			$<$<BOOL:${ERM_OPEN_GL}>:GL_SILENCE_DEPRECATION>
	)

	target_compile_options(
		"${PROJECT_NAME}"
		PRIVATE
			$<$<BOOL:${ERM_DEBUG}>:-Wall -Werror -Wextra -pedantic-errors -Wno-gnu>
	)

	if(ERM_VULKAN)
		set(SHADERS_COMPILER "glslc" PARENT_SCOPE)
		set(ERM_FLIP_VIEWPORT ON CACHE BOOL "" FORCE)
		set(ERM_FLIP_PROJECTION OFF CACHE BOOL "" FORCE)
	else()
		set(ERM_FLIP_VIEWPORT OFF CACHE BOOL "" FORCE)
		set(ERM_FLIP_PROJECTION OFF CACHE BOOL "" FORCE)
	endif()
endfunction()

