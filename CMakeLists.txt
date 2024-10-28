cmake_minimum_required(VERSION 3.5)
project(WebServer)
add_executable(WebServer)

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/utils")

aux_source_directory("${CMAKE_CURRENT_SOURCE_DIR}/" SRC_LIST)
target_sources(WebServer PRIVATE ${SRC_LIST})

target_compile_options(WebServer PRIVATE -std=c++11)
target_compile_options(WebServer PUBLIC -g)