cmake_minimum_required(VERSION 4.0)
include(FetchContent)

project(Cloud++)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(Cloud++)

FetchContent_Declare(
    tomlplusplus
    GIT_REPOSITORY https://github.com/marzer/tomlplusplus.git
    GIT_TAG        v3.4.0
)
FetchContent_MakeAvailable(tomlplusplus)

target_include_directories(Cloud++ PRIVATE
  /usr/include/mysql
  /usr/include/mysql++
  ./libs
)

target_link_libraries(Cloud++ PRIVATE
  mysqlpp
  mariadb
  tomlplusplus::tomlplusplus
)

file(GLOB SRCS
  ./src/*.cpp
  ./src/*.hpp
  ./src/commands/*.cpp
  ./src/commands/*.hpp
)

target_sources(Cloud++
  PRIVATE 
    ${SRCS}
)
