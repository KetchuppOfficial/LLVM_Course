cmake_minimum_required(VERSION 3.21)

project(llvm-course
        LANGUAGES C CXX)

if (${PROJECT_SOURCE_DIR} STREQUAL ${PROJECT_BINARY_DIR})
        message(FATAL_ERROR "In-source build is forbidden")
endif()

if (NOT ${CMAKE_C_COMPILER_ID} STREQUAL "Clang")
    message(FATAL_ERROR "Only clang may be used as C compiler")
endif()

if (NOT ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
    message(FATAL_ERROR "Only clang may be used as C++ compiler")
endif()

if (NOT ${CMAKE_C_COMPILER_VERSION} STREQUAL ${CMAKE_CXX_COMPILER_VERSION})
    message(FATAL_ERROR "Use clang and clang++ of the same version")
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(LLVM 18.1.3 REQUIRED)
find_package(SDL2 REQUIRED)

include(GNUInstallDirs)

add_subdirectory(00-graphics-application)
add_subdirectory(01-pass)
add_subdirectory(02-ir-builder)
