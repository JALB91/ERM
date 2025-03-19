project(
	"ERM_Libs"
	VERSION 1.0.0
	DESCRIPTION "ERM Libraries"
)

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/ERMLibs.cmake")

set(ERM_GENERATED_DIR "${CMAKE_INSTALL_PREFIX}/generated")

erm_gather_libs(ERM_LIBS_DIRECTORIES)

foreach(ERM_LIB_DIRECTORY ${ERM_LIBS_DIRECTORIES})
    erm_gather_sources("${ERM_LIB_DIRECTORY}")
endforeach()

foreach(ERM_LIB_DIRECTORY ${ERM_LIBS_DIRECTORIES})
    add_subdirectory("${ERM_LIB_DIRECTORY}")
endforeach()
