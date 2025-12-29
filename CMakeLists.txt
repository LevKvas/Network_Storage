cmake_minimum_required(VERSION 3.16)
project(NetworkStorage)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SFML_ROOT "C:/Users/USER/Desktop/c/Network_Storage/SFML-2.6.2")

include_directories(${SFML_ROOT}/include)

link_directories(${SFML_ROOT}/lib)

#add_executable(NetworkStorage server_main.cpp)
#
#target_link_libraries(NetworkStorage
#        sfml-network
#        sfml-system
#)

add_executable(server server_main.cpp)

target_link_libraries(server
        sfml-network
        sfml-system
)

add_executable(client client_main.cpp src/Client.cpp)

target_link_libraries(client
        sfml-network
        sfml-system
)