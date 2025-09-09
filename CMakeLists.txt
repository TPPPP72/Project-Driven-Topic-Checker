cmake_minimum_required(VERSION 3.10)
project(Task_Checker)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")

aux_source_directory(src SRC_FILES)

add_executable(${PROJECT_NAME} ${SRC_FILES})