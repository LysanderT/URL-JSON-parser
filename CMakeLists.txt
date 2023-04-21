cmake_minimum_required(VERSION 3.25)
project(screening C)

set(CMAKE_C_STANDARD 11)

include_directories(.)
include_directories(third_party/cJSON)

add_executable(screening
        third_party/cJSON/cJSON.c
        third_party/cJSON/cJSON.h
        main.c
        parse.c
        parse.h)
