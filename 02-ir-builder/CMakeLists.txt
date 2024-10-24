cmake_minimum_required(VERSION 3.21)

project(ir-builder)

if (${PROJECT_SOURCE_DIR} STREQUAL ${PROJECT_BINARY_DIR})
    message(FATAL_ERROR "In-source build is forbidden")
endif()

find_package(LLVM 18.1.3 REQUIRED)

set(SOURCE_DIR ${PROJECT_SOURCE_DIR}/src)
set(APP_DIR ${PROJECT_SOURCE_DIR}/../00-graphics-application)
set(APP_SOURCE_DIR ${APP_DIR}/src)
set(APP_INCLUDE_DIR ${APP_DIR}/include)

add_executable(ir_builder
    ${SOURCE_DIR}/builder.cpp
    ${APP_SOURCE_DIR}/graphics.c)
target_include_directories(ir_builder PRIVATE ${LLVM_INCLUDE_DIRS} ${APP_INCLUDE_DIR})
target_link_libraries(ir_builder PRIVATE LLVM SDL2 m)
set_target_properties(ir_builder PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF)
