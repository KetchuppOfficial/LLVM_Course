cmake_minimum_required(VERSION 3.21)

project(lloyd-relaxation
        LANGUAGES C)

if (${PROJECT_SOURCE_DIR} STREQUAL ${PROJECT_BINARY_DIR})
    message(FATAL_ERROR "In-source build is forbidden")
endif()

if (NOT ${CMAKE_C_COMPILER_ID} STREQUAL "Clang")
    message(FATAL_ERROR "Only clang may be used as C compiler")
endif()

set(CMAKE_C_STANDARD          17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS        OFF)

set(SOURCE_DIR ${PROJECT_SOURCE_DIR}/src)
set(INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
set(INSTALL_DIR ${PROJECT_SOURCE_DIR}/bin)

find_package(SDL2 REQUIRED)

aux_source_directory(${SOURCE_DIR} SRC)

add_executable(lloyd_relaxation ${SRC})

target_link_libraries(lloyd_relaxation PRIVATE SDL2 m)
target_include_directories(lloyd_relaxation PRIVATE ${INCLUDE_DIR})

add_library(lloyd_relaxation_ir OBJECT ${SOURCE_DIR}/lloyd_relaxation.c)

target_include_directories(lloyd_relaxation_ir PRIVATE ${INCLUDE_DIR})
target_compile_options(lloyd_relaxation_ir PRIVATE ${CMAKE_C_FLAGS} -S -emit-llvm)

install(TARGETS lloyd_relaxation lloyd_relaxation_ir
        DESTINATION ${INSTALL_DIR})
