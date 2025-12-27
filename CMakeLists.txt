cmake_minimum_required(VERSION 3.30)
project(NetworkStorage)

set(CMAKE_CXX_STANDARD 20)
include_directories(includes)

# SFML
include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/sfml-simple/include
        # ИЛИ
        ${CMAKE_CURRENT_SOURCE_DIR}/includes
)

add_executable(NetworkStorage main.cpp)

# chain libs
target_link_libraries(NetworkStorage
        ${CMAKE_CURRENT_SOURCE_DIR}/sfml-simple/lib/libsfml-network.a
        ${CMAKE_CURRENT_SOURCE_DIR}/sfml-simple/lib/libsfml-system.a
        ws2_32
)
