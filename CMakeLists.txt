cmake_minimum_required(VERSION 3.10)

project(toyjsruntime C)

#set(CMAKE_C_COMPILER /usr/bin/clang)
#set(CMAKE_CXX_COMPILER /usr/bin/clang++)

set(TOY_COMPILE_OPTIONS
        -Wall
        -Wextra
)


set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build type" FORCE)
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -O0 -g -DDEBUG")


set(TOYJSRUNTIME_CMAKE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(TOYJSRUNTIME_CMAKE_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})

# Find all source files in src directory and src/modules directory
file(GLOB SRC_FILES
        "${CMAKE_SOURCE_DIR}/src/modules/*.c"
)

# Compile JS files to C  via make generate_bundles
file(GLOB BUNDLES_JS "${CMAKE_SOURCE_DIR}/src/bundles/*.js")
set(GENERATED_C_FILES "")
foreach (js_file ${BUNDLES_JS})
    get_filename_component(js_name ${js_file} NAME_WE)
    set(c_file "${CMAKE_SOURCE_DIR}/src/bundles/${js_name}.c")
    add_custom_command(
            OUTPUT ${c_file}
            COMMAND ${CMAKE_BINARY_DIR}/toyjsruntimec -o ${c_file} ${js_file}
            DEPENDS ${js_file} ${CMAKE_SOURCE_DIR}/build/quickjs/qjsc
            COMMENT "Compiling ${js_file} to ${c_file}"
    )
    list(APPEND GENERATED_C_FILES ${c_file})
endforeach ()

# Create custom target for generating bundles
add_custom_target(generate_bundles DEPENDS ${GENERATED_C_FILES})

# libs
# Add libuv submodule
set(LIBUV_BUILD_TESTS OFF)
set(LIBUV_DIR ${CMAKE_SOURCE_DIR}/deps/libuv)
# Include libuv as a subdirectory
add_subdirectory(${LIBUV_DIR} ${CMAKE_BINARY_DIR}/libuv)

# Add quickjs submodule similarly to libuv
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(QUICKJS_DIR ${CMAKE_SOURCE_DIR}/deps/quickjs)
set(BUILD_QJS_LIBC ON) # default quickjs CMakeLists is use option to control this
add_subdirectory(${QUICKJS_DIR} ${CMAKE_BINARY_DIR}/quickjs)

message(STATUS "CMAKE_SOURCE_DIR: ${CMAKE_SOURCE_DIR}")
message(STATUS "CMAKE_BINARY_DIR: ${CMAKE_BINARY_DIR}")

# Find and link CURL library via sysytem
find_package(PkgConfig REQUIRED)
find_package(CURL REQUIRED)

pkg_check_modules(GTK4 REQUIRED IMPORTED_TARGET gtk4)
find_library(FFI_LIB NAMES libffi ffi REQUIRED)

# libary 
add_library(testayntax src/test/sytax.c)
target_compile_options(testayntax PRIVATE ${TOY_COMPILE_OPTIONS})

# target
add_executable(toyjsruntime ${CMAKE_SOURCE_DIR}/src/main.c ${SRC_FILES})
add_executable(toyjsruntimec ${CMAKE_SOURCE_DIR}/src/toyjsc.c ${SRC_FILES})
add_executable(testquickjs src/test/main.c ${SRC_FILES})

# only fot test
add_executable(gtk-test src/test/gtk.c)

#library
add_library(test_lib SHARED example/ffi/test-lib.c)

# target_link_libraries(quickjs-ffi PRIVATE qjs ${FFI_LIB})
# target_include_directories(${PROJECT_NAME} PRIVATE ${CURL_INCLUDE_DIRS} )

target_compile_options(toyjsruntime PRIVATE ${TOY_COMPILE_OPTIONS})
target_link_libraries(toyjsruntime PRIVATE m ${CURL_LIBRARIES} uv qjs ${FFI_LIB})
target_link_libraries(toyjsruntimec PRIVATE m ${CURL_LIBRARIES} uv qjs ${FFI_LIB})

target_link_libraries(testquickjs PRIVATE m ${CURL_LIBRARIES} uv qjs ${FFI_LIB})
target_link_libraries(gtk-test PRIVATE PkgConfig::GTK4)

# Output CURL include directories
message(STATUS "CURL include directories: ${CURL_INCLUDE_DIRS}")
message(STATUS "GTK4 include directories: ${GTK4_INCLUDE_DIRS}")


