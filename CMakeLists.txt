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

# libs
# Add libuv submodule
set(LIBUV_DIR ${CMAKE_SOURCE_DIR}/deps/libuv)
# Include libuv as a subdirectory
add_subdirectory(${LIBUV_DIR} ${CMAKE_BINARY_DIR}/libuv)

# Output CURL include directories
message(STATUS "CMAKE_SOURCE_DIR: ${CMAKE_SOURCE_DIR}")
message(STATUS "CMAKE_BINARY_DIR: ${CMAKE_BINARY_DIR}")


# target
add_executable( ${PROJECT_NAME} ${TOYJSRUNTIME_CMAKE_SOURCES} )

# Add necessary compiler definitions for QuickJS
target_compile_definitions(${PROJECT_NAME} PRIVATE 
    _GNU_SOURCE
    CONFIG_VERSION="${PROJECT_VERSION}"
)

# Include QuickJS headers
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/deps/quickjs)
target_include_directories(${PROJECT_NAME} PRIVATE ${LIBUV_DIR}/include)

# Link against the math library
target_link_libraries(${PROJECT_NAME} PRIVATE m)

# Find and link CURL library
find_package(CURL REQUIRED)
target_include_directories(${PROJECT_NAME} PRIVATE ${CURL_INCLUDE_DIRS} )
target_link_libraries(${PROJECT_NAME} PRIVATE m ${CURL_LIBRARIES} uv)

# Output CURL include directories
message(STATUS "CURL include directories: ${CURL_INCLUDE_DIRS}")


