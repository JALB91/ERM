function(modules_post_setup_callback)
	foreach(ERM_MODULE ${ERM_MODULES})
		if("${ERM_MODULE}" STREQUAL "ERM_Modules")
			continue()
		endif()

		# Add ERM_Modules_shared linking for all modules except standalone ones
		erm_get_module_property(MODULE_STANDALONE ${ERM_MODULE} STANDALONE)
		
		if(NOT MODULE_STANDALONE)
			erm_add_to_module_property(
				TARGET ${ERM_MODULE}
				PROPERTY LINK_PUBLIC
				VALUES ERM_Modules_shared
			)
		endif()
	endforeach()
endfunction()
