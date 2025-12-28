cmake_minimum_required(VERSION 3.16)
project(NetworkStorage)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SFML_ROOT "C:/Users/USER/Desktop/c/Network_Storage/SFML-2.6.2")

include_directories(${SFML_ROOT}/include)

link_directories(${SFML_ROOT}/lib)

add_executable(NetworkStorage main.cpp)

target_link_libraries(NetworkStorage
        sfml-network
        sfml-system
)