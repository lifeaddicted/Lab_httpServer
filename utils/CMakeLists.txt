cmake_minimum_required(VERSION 3.5)
add_library(utils)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SRC)
target_sources(utils PUBLIC ${SRC})

target_link_libraries(utils PRIVATE -lpthread)

target_compile_options(utils PRIVATE -std=c++11)