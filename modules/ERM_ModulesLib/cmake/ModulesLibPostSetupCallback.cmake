function(modules_lib_post_setup_callback)
	foreach(ERM_MODULE IN LISTS ERM_MODULES)
		erm_get_module_property(MODULE_STANDALONE ${ERM_MODULE} STANDALONE)

		# Add ERM_ModulesLib_shared linking for all modules except standalone ones
		if(NOT MODULE_STANDALONE)
			erm_add_to_module_property(
				TARGET ${ERM_MODULE}
				PROPERTY LINK_PUBLIC
				VALUES ERM_ModulesLib_shared
			)
		endif()
	endforeach()
endfunction()
