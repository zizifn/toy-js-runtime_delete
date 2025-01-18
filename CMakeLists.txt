cmake_minimum_required (VERSION 3.10)

project (toyjsruntime C)

set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build type" FORCE)
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -O0 -g")

set( TOYJSRUNTIME_CMAKE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR} )
set( TOYJSRUNTIME_CMAKE_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR} )

# Find all source files in src directory and src/modules directory
file(GLOB SRC_FILES 
    "${CMAKE_SOURCE_DIR}/src/modules/*.c"
)


# libs
# Add libuv submodule
set(LIBUV_DIR ${CMAKE_SOURCE_DIR}/deps/libuv)
# Include libuv as a subdirectory
add_subdirectory(${LIBUV_DIR} ${CMAKE_BINARY_DIR}/libuv)

# Add quickjs submodule similarly to libuv
set(QUICKJS_DIR ${CMAKE_SOURCE_DIR}/deps/quickjs)
set(BUILD_QJS_LIBC ON) # default quickjs CMakeLists is use option to control this
add_subdirectory(${QUICKJS_DIR} ${CMAKE_BINARY_DIR}/quickjs)

message(STATUS "CMAKE_SOURCE_DIR: ${CMAKE_SOURCE_DIR}")
message(STATUS "CMAKE_BINARY_DIR: ${CMAKE_BINARY_DIR}")

# Find and link CURL library
find_package(CURL REQUIRED)

# target
add_executable( ${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/src/main.c ${SRC_FILES} )
add_executable(testquickjs src/test/main.c ${SRC_FILES})

# target_include_directories(${PROJECT_NAME} PRIVATE ${CURL_INCLUDE_DIRS} )

target_link_libraries(${PROJECT_NAME} PRIVATE m ${CURL_LIBRARIES} uv qjs)
target_link_libraries(testquickjs PRIVATE m ${CURL_LIBRARIES} uv qjs)

# Output CURL include directories
message(STATUS "CURL include directories: ${CURL_INCLUDE_DIRS}")


