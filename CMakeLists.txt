cmake_minimum_required(VERSION 3.2)

project(Interpreter)

# Configure compiler flags for our project
if (CMAKE_CXX_COMPILER_ID MATCHES Clang OR ${CMAKE_CXX_COMPILER_ID} STREQUAL GNU)
  # using regular Clang or AppleClang or GCC
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wall -Wextra -pedantic -Wold-style-cast -lstdc++fs -O0")
elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
  # using Visual Studio C++
  set(CMAKE_CXX_FLAGS "/std:c++17 /permissive- /W4 /EHsc")
endif()

# Copying of resources to output directories
file(GLOB RESOURCES ${CMAKE_SOURCE_DIR}/examples/*.ct)
file(COPY ${RESOURCES} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/examples)

FILE(GLOB TESTS tests/*.cpp)

# Add executable
add_executable(InterpretCLI Cli.cpp Ast.cpp)
add_executable(Test ${TESTS} Ast.cpp)

file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/examples" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")