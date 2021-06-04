cmake_minimum_required(VERSION 3.17)
project(OShw3)

set(CMAKE_CXX_STANDARD 14)

add_executable(OShw3 output.c request.c request.h segel.c segel.h server.c client.c queue.c queue.h clientStatic.c clientStatic.h clientDynamic.c clientDynamic.h clientError.c clientError.h)
