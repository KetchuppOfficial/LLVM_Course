cmake_minimum_required(VERSION 3.21)

project(lloyd-relaxation
        LANGUAGES C)

if (${PROJECT_SOURCE_DIR} STREQUAL ${PROJECT_BINARY_DIR})
    message(FATAL_ERROR "In-source build is forbidden")
endif()

find_package(SDL2 REQUIRED)

set(CMAKE_C_STANDARD          17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS        OFF)

add_executable(lloyd_relaxation ./src/main.c
                                ./src/point.c
                                ./src/line.c
                                ./src/graphics.c
                                ./src/lloyd_relaxation.c)

target_link_libraries(lloyd_relaxation PRIVATE SDL2 m)
target_include_directories(lloyd_relaxation PRIVATE ${PROJECT_SOURCE_DIR}/include)
