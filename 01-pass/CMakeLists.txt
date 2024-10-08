cmake_minimum_required(VERSION 3.21)

project(tracing-pass)

if (${PROJECT_SOURCE_DIR} STREQUAL ${PROJECT_BINARY_DIR})
    message(FATAL_ERROR "In-source build is forbidden")
endif()

if (NOT ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
    message(FATAL_ERROR "Only clang may be used as C++ compiler")
endif()

if (NOT ${CMAKE_C_COMPILER_ID} STREQUAL "Clang")
    message(FATAL_ERROR "Only clang may be used as C compiler")
endif()

set(CMAKE_CXX_STANDARD          23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS        OFF)

set(CMAKE_C_STANDARD          17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS        OFF)

find_package(LLVM 18.1.3 REQUIRED)
find_package(SDL2 REQUIRED)

set(PASS_SOURCE_DIR ${PROJECT_SOURCE_DIR}/src)
set(APP_DIR ${PROJECT_SOURCE_DIR}/../00-graphics_application)
set(APP_SOURCE_DIR ${APP_DIR}/src)
set(APP_INCLUDE_DIR ${APP_DIR}/include)

add_library(InstrTracerPass SHARED ${PASS_SOURCE_DIR}/pass.cpp)
target_include_directories(InstrTracerPass PRIVATE ${LLVM_INCLUDE_DIRS})

add_library(app_logic OBJECT ${APP_SOURCE_DIR}/lloyd_relaxation.c)
add_dependencies(app_logic InstrTracerPass)
target_include_directories(app_logic PRIVATE ${APP_INCLUDE_DIR})
target_compile_options(app_logic PRIVATE -fpass-plugin=$<TARGET_FILE:InstrTracerPass>)

add_executable(lloyd_relaxation_instrumented
    ${APP_SOURCE_DIR}/main.c
    ${APP_SOURCE_DIR}/graphics.c
    ${PASS_SOURCE_DIR}/logger.cpp
    $<TARGET_OBJECTS:app_logic>)
target_include_directories(lloyd_relaxation_instrumented PRIVATE
    ${APP_INCLUDE_DIR}
    ${LLVM_INCLUDE_DIRS})
target_link_libraries(lloyd_relaxation_instrumented PRIVATE LLVM SDL2 m)
