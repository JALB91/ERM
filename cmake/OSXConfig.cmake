function(target_setup_project TARGET)
	set_target_properties(${TARGET} PROPERTIES
		XCODE_ATTRIBUTE_CONFIGURATION_BUILD_DIR "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
  		XCODE_SCHEME_WORKING_DIRECTORY "${CMAKE_INSTALL_PREFIX}"
		XCODE_GENERATE_SCHEME TRUE
	)

	target_compile_definitions(
		"${TARGET}"
		PRIVATE 
			$<$<STREQUAL:"${TARGET_API}","OpenGl">:GL_DO_NOT_WARN_IF_MULTI_GL_VERSION_HEADERS_INCLUDED>
			$<$<STREQUAL:"${TARGET_API}","OpenGl">:GL_SILENCE_DEPRECATION>
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
