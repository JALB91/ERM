function(modules_lib_pre_generate_callback)
	erm_get_module_properties(
		TARGET ERM_ModulesLib
		PREFIX ERM_MODULES
		PROPERTIES 
			BASE_DIR 
			GENERATED_DIR
	)
	set(AllModulesIncludes "")
	set(AllModulesList "")

	foreach(ERM_MODULE IN LISTS ERM_MODULES)
		erm_get_module_properties(
			PREFIX MODULE 
			TARGET ${ERM_MODULE}
			PROPERTIES
				NAME
				BASE_DIR
				GENERATED_DIR
				DESCRIPTION
				VERSION_MAJOR
				VERSION_MINOR
				VERSION_PATCH
				ALL_DEPS
				STANDALONE
				EXECUTABLE
		)
		
		string(APPEND AllModulesIncludes "#include <erm/${ERM_MODULE}.h>\n")
		string(APPEND AllModulesList "\n\t${ERM_MODULE},")

		set(ModuleDependenciesForwardDeclarations "")
		set(ModuleDependenciesIncludes "#include \"erm/${ERM_MODULE}.h\"\n\n")
		set(ModuleDependenciesList "")
		set(ModuleName "${ERM_MODULE}")

		foreach(DEP IN LISTS MODULE_ALL_DEPS)
			string(REPLACE "_static" "" DEP ${DEP})
			string(REPLACE "_shared" "" DEP ${DEP})
			string(REPLACE "_int" "" DEP ${DEP})
			string(REPLACE "_obj" "" DEP ${DEP})

			if(NOT TARGET ${DEP})
				continue()
			endif()

			erm_get_module_property(DEP_STANDALONE ${DEP} STANDALONE)
			if(NOT DEFINED DEP_STANDALONE OR DEP_STANDALONE)
				continue()
			endif()

			list(FIND ERM_MODULES ${DEP} INDEX)

			if(${INDEX} GREATER_EQUAL 0)
				string(APPEND ModuleDependenciesForwardDeclarations "class ${DEP};\n")
				string(APPEND ModuleDependenciesIncludes "#include <erm/${DEP}.h>\n")
				string(APPEND ModuleDependenciesList "${DEP}, ")
			endif()
		endforeach()

		if(ModuleDependenciesForwardDeclarations)
			erm_string_substr_end("${ModuleDependenciesForwardDeclarations}" 1 ModuleDependenciesForwardDeclarations)
		endif()

		if(ModuleDependenciesIncludes)
			erm_string_substr_end("${ModuleDependenciesIncludes}" 1 ModuleDependenciesIncludes)
		endif()
		
		if(ModuleDependenciesList)
			erm_string_substr_end("${ModuleDependenciesList}" 2 ModuleDependenciesList)
		endif()

		# Configure ERM module header
		if(MODULE_STANDALONE)
			set(MODULE_HEADER_TEMPLATE "${ERM_MODULES_BASE_DIR}/Templates/ERM_ModuleStandalone.h")
		elseif(MODULE_EXECUTABLE)
			set(MODULE_HEADER_TEMPLATE "${ERM_MODULES_BASE_DIR}/Templates/ERM_ModuleExecutable.h")
		else()
			set(MODULE_HEADER_TEMPLATE "${ERM_MODULES_BASE_DIR}/Templates/ERM_Module.h")
		endif()

		set(MODULE_HEADER_PATH "${MODULE_GENERATED_DIR}/common/erm/${ERM_MODULE}.h")
		configure_file(
			"${MODULE_HEADER_TEMPLATE}"
			"${MODULE_HEADER_PATH}"
			@ONLY
		)
		
		erm_add_to_module_property(
			TARGET ${ERM_MODULE}
			PROPERTY GENERATED_CPP_HEADERS
			VALUES "${MODULE_HEADER_PATH}"
		)

		erm_add_to_module_property(
			TARGET ERM_ModulesLib
			PROPERTY PUBLIC_INCLUDE_DIRS
			VALUES "${MODULE_GENERATED_DIR}/common"
		)

		erm_add_to_module_property(
			TARGET ERM_ModulesLib
			PROPERTY GENERATED_CPP_HEADERS
			VALUES "${MODULE_HEADER_PATH}"
		)

		erm_get_module_property(MODULE_EXECUTABLE ${ERM_MODULE} EXECUTABLE)
		if(MODULE_EXECUTABLE)
			# Configure dummy_main.cpp
			set(MAIN_PATH "${MODULE_GENERATED_DIR}/common/erm/${ERM_MODULE}_main.cpp")
			configure_file(
				"${ERM_MODULES_BASE_DIR}/Templates/module_main.cpp"
				"${MAIN_PATH}"
				@ONLY
			)
			erm_add_to_module_property(
				TARGET ${ERM_MODULE}
				PROPERTY MODULE_MAIN_PATH
				VALUES "${MAIN_PATH}"
			)
		endif()
	endforeach()

	# Configure AllHeaders.h
	if(AllModulesList)
		erm_string_substr_end("${AllModulesList}" 1 AllModulesList)
	endif()

	if(AllModulesIncludes)
		erm_string_substr_end("${AllModulesIncludes}" 1 AllModulesIncludes)
	endif()

	set(ALL_MODULES_PATH "${ERM_MODULES_GENERATED_DIR}/common/erm/AllModules.h")
	configure_file(
			"${ERM_MODULES_BASE_DIR}/Templates/AllModules.h"
			"${ALL_MODULES_PATH}"
			@ONLY
	)
	erm_add_to_module_property(
		TARGET ERM_ModulesLib
		PROPERTY GENERATED_CPP_HEADERS
		VALUES "${ALL_MODULES_PATH}"
	)
endfunction()
