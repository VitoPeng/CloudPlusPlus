cmake_minimum_required(VERSION 4.0)
project(Cloud++)

add_executable(Cloud++)

set(CMAKE_CXX_COMPILER g++)
set(CMAKE_CXX_STANDARD 20)

target_compile_options(Cloud++ PRIVATE -Wall -g)

file(GLOB_RECURSE SRCS
  src/*.cpp
  src/*.hpp
)

target_sources(Cloud++
  PRIVATE 
    ${SRCS}
)
