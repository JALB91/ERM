project(
	"ERM_Libs"
	VERSION 1.0.0
	DESCRIPTION "ERM Libraries"
)

set(ERM_GENERATED_DIR "${CMAKE_INSTALL_PREFIX}/generated")

file(GLOB ERM_LIBS_DIRECTORIES LIST_DIRECTORIES true *)

foreach(ERM_LIB_DIRECTORY ${ERM_LIBS_DIRECTORIES})
    if(IS_DIRECTORY "${ERM_LIB_DIRECTORY}")
        erm_gather_sources("${ERM_LIB_DIRECTORY}")
        if(NN_STATEMENTS)
            list(APPEND NN_ALL_STATEMENTS ${NN_STATEMENTS})
        endif()
        set(NN_ALL_STATEMENTS ${NN_ALL_STATEMENTS})
    endif()
endforeach()

foreach(ERM_LIB_DIRECTORY ${ERM_LIBS_DIRECTORIES})
    if(IS_DIRECTORY "${ERM_LIB_DIRECTORY}")
        add_subdirectory("${ERM_LIB_DIRECTORY}")
    endif()
endforeach()
