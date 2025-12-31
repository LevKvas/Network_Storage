cmake_minimum_required(VERSION 3.16)
project(NetworkStorage)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion -Wcast-qual -O3)
endif()

set(SFML_ROOT "C:/Users/USER/Desktop/c/Network_Storage/SFML-2.6.2")

include_directories(${SFML_ROOT}/include)

link_directories(${SFML_ROOT}/lib)


add_executable(server server_main.cpp)

target_link_libraries(server
        sfml-network
        sfml-system
)

add_subdirectory(googletest)
add_executable(client client_main.cpp src/Client.cpp)

add_executable(tests tests/main_tests.cpp tests/server_tests.cpp src/Client.cpp)
target_link_libraries(client
        sfml-network
        sfml-system
)
target_link_libraries(tests
        sfml-network
        sfml-system
        gtest
        gtest_main)


