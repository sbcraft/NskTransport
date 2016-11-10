cmake_minimum_required(VERSION 3.6)
project(NskTransport)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
    src/app.c)

add_executable(NskTransport ${SOURCE_FILES})