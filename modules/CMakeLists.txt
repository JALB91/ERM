project(
	"ERM_Modules"
	VERSION 1.0.0
	DESCRIPTION "ERM Modules"
)

# Updates cmake module path
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Include erm modules and utils
include(ERMModulesUtils)
include(ERMModules)

set(ERM_GENERATED_DIR "${CMAKE_INSTALL_PREFIX}/generated" CACHE STRING "" FORCE)

erm_gather_modules(ERM_MODULES_DIRECTORIES)

foreach(ERM_MODULE_DIRECTORY ${ERM_MODULES_DIRECTORIES})
    erm_gather_sources("${ERM_MODULE_DIRECTORY}")
endforeach()

foreach(ERM_MODULE_DIRECTORY ${ERM_MODULES_DIRECTORIES})
    add_subdirectory("${ERM_MODULE_DIRECTORY}")
endforeach()
