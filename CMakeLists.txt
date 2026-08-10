cmake_minimum_required(VERSION 4.0)

project(Cloud++)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(Cloud++)

target_compile_options(Cloud++ PRIVATE -Wall -g)

file(GLOB_RECURSE SRCS
  src/*.cpp
  src/*.hpp
  src/**/*.cpp
  src/**/*.hpp
)

target_sources(Cloud++
  PRIVATE 
    ${SRCS}
)
