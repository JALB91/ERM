function(target_setup_project TARGET)
	target_compile_definitions(
		"${TARGET}"
		PRIVATE 
			GL_DO_NOT_WARN_IF_MULTI_GL_VERSION_HEADERS_INCLUDED
			GL_SILENCE_DEPRECATION
	)

	target_compile_options(
		"${TARGET}"
		PRIVATE
			$<$<STREQUAL:"${CMAKE_BUILD_TYPE}","Debug">:-Wall -Werror -Wextra -pedantic-errors>
	)

	if("${TARGET_API}" STREQUAL "Vulkan")
		set(SHADERS_COMPILER "glslc" PARENT_SCOPE)
	endif("${TARGET_API}" STREQUAL "Vulkan")
endfunction()
