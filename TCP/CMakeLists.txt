cmake_minimum_required(VERSION 3.6)
project(TCP)

set(CMAKE_CXX_STANDARD 11)

add_executable(server tcp.c tcp.h server.c)
add_executable(client client.c tcp.h client.c)