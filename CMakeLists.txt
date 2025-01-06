cmake_minimum_required (VERSION 3.10)

project (toyjsruntime C)

set( TOYJSRUNTIME_CMAKE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR} )
set( TOYJSRUNTIME_CMAKE_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR} )

# Define QuickJS sources
set(QUICKJS_SOURCES
    ${CMAKE_SOURCE_DIR}/deps/quickjs/quickjs.c
    ${CMAKE_SOURCE_DIR}/deps/quickjs/libregexp.c
    ${CMAKE_SOURCE_DIR}/deps/quickjs/libunicode.c
    ${CMAKE_SOURCE_DIR}/deps/quickjs/cutils.c
    ${CMAKE_SOURCE_DIR}/deps/quickjs/libbf.c
    ${CMAKE_SOURCE_DIR}/deps/quickjs/quickjs-libc.c
)

# Find all source files in src directory and src/modules directory
file(GLOB SRC_FILES 
    "${CMAKE_SOURCE_DIR}/src/*.c"
    "${CMAKE_SOURCE_DIR}/src/modules/*.c"
)

set( TOYJSRUNTIME_CMAKE_SOURCES 
    ${SRC_FILES}
    ${QUICKJS_SOURCES}
)

# target
add_executable( ${PROJECT_NAME} ${TOYJSRUNTIME_CMAKE_SOURCES} )

# Add necessary compiler definitions for QuickJS
target_compile_definitions(${PROJECT_NAME} PRIVATE 
    _GNU_SOURCE
    CONFIG_VERSION="${PROJECT_VERSION}"
)

# Include QuickJS headers
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/deps/quickjs)

# Link against the math library
target_link_libraries(${PROJECT_NAME} PRIVATE m)

