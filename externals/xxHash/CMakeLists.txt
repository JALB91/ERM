project("xxHash")

add_library(
    ${PROJECT_NAME} 
    STATIC 
        "${CMAKE_CURRENT_SOURCE_DIR}/xxHash/xxHash.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/xxHash/xxHash.h"
)

target_include_directories(
    ${PROJECT_NAME}
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/xxHash/"
)