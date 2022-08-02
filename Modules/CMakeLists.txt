# Cmake version
cmake_minimum_required(VERSION 3.15...3.17)

file(GLOB MODULES LIST_DIRECTORIES true *)

foreach(MODULE ${MODULES})
    if(IS_DIRECTORY "${MODULE}")
        add_subdirectory("${MODULE}")
    endif()
endforeach()
