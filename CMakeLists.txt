cmake_minimum_required(VERSION 3.10)
project(Task_Checker)

set(CMAKE_CXX_STANDARD 20)

aux_source_directory(src SRC_FILES)

add_executable(Task_Checker ${SRC_FILES})
