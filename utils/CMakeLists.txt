cmake_minimum_required(VERSION 3.5)
add_library(utils OBJECT)

aux_source_directory("${CMAKE_CURRENT_SOURCE_DIR}" SRC_LIST)
target_sources(utils PRIVATE  SRC_LIST)

target_compile_options(utils PRIVATE -std=c++11)