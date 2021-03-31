function(target_setup_project TARGET)
	set_target_properties(${TARGET} PROPERTIES
		XCODE_ATTRIBUTE_CONFIGURATION_BUILD_DIR "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
  		XCODE_SCHEME_WORKING_DIRECTORY "${CMAKE_INSTALL_PREFIX}"
		XCODE_GENERATE_SCHEME TRUE
	)

	target_compile_definitions(
		"${TARGET}"
		PRIVATE 
			ERM_OSX
			$<$<STREQUAL:"${TARGET_API}","OpenGl">:GL_DO_NOT_WARN_IF_MULTI_GL_VERSION_HEADERS_INCLUDED>
			$<$<STREQUAL:"${TARGET_API}","OpenGl">:GL_SILENCE_DEPRECATION>
	)

	target_compile_options(
		"${TARGET}"
		PRIVATE
			$<$<STREQUAL:"${CMAKE_BUILD_TYPE}","Debug">:-Wall -Werror -Wextra -pedantic-errors -Wno-gnu>
	)

	if("${TARGET_API}" STREQUAL "Vulkan")
		set(SHADERS_COMPILER "glslc --target-spv=spv1.3 -I ${RES_DEST}/shaders/Vulkan/imports/" PARENT_SCOPE)
		set(ERM_FLIP_VIEWPORT ON CACHE BOOL "" FORCE)
		set(ERM_FLIP_PROJECTION OFF CACHE BOOL "" FORCE)
	else()
		set(ERM_FLIP_VIEWPORT OFF CACHE BOOL "" FORCE)
		set(ERM_FLIP_PROJECTION OFF CACHE BOOL "" FORCE)
	endif()
endfunction()

