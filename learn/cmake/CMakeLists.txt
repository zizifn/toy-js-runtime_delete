cmake_minimum_required (VERSION 3.10)


# 在这里定义了项目的名称，同时会影响Cmake 生成的目录命名约定。
# 我们可以将代码的语言作为第二个参数传入。
project (learncmake C)

# 设置项目的源目录（仅仅是由于惯例）
set( LEARN_CMAKE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR} )
set( LEARN_CMAKE_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR} )

set( LEARN_CMAKE_SOURCES 
  src/main.c
  src/lib.c
)

add_executable( ${PROJECT_NAME} ${LEARN_CMAKE_SOURCES} )
target_link_libraries( ${PROJECT_NAME} ${LIBS} m )
