cmake_minimum_required(VERSION 3.21)

project(tracing-pass
        LANGUAGES C CXX)

if (${PROJECT_SOURCE_DIR} STREQUAL ${PROJECT_BINARY_DIR})
    message(FATAL_ERROR "In-source build is forbidden")
endif()

if (NOT ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
    message(FATAL_ERROR "Only clang may be used as C++ compiler")
endif()

if (NOT ${CMAKE_C_COMPILER_ID} STREQUAL "Clang")
    message(FATAL_ERROR "Only clang may be used as C compiler")
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(LLVM 18.1.3 REQUIRED)
find_package(SDL2 REQUIRED)

# shared library with instrumenting LLVM pass
add_library(instr_tracer_pass SHARED ./src/pass.cpp)
target_include_directories(instr_tracer_pass PRIVATE ${LLVM_INCLUDE_DIRS})


# application logic compiled with the instrumenting pass
set(APP_DIR ${PROJECT_SOURCE_DIR}/../00-graphics-application)
add_library(app_logic OBJECT ${APP_DIR}/src/lloyd_relaxation.c)
add_dependencies(app_logic instr_tracer_pass)
target_include_directories(app_logic PRIVATE ${APP_DIR}/include)
target_compile_options(app_logic PRIVATE -fpass-plugin=$<TARGET_FILE:instr_tracer_pass>)


# final instrumented graphics application
add_executable(lloyd_relaxation_instrumented
    ./src/logger.cpp
    ${APP_DIR}/src/main.c
    ${APP_DIR}/src/graphics.c
)

target_link_libraries(lloyd_relaxation_instrumented
PRIVATE
    LLVM
    SDL2
    $<TARGET_OBJECTS:app_logic>
)

target_include_directories(lloyd_relaxation_instrumented
PRIVATE
    ${LLVM_INCLUDE_DIRS}
    ${APP_DIR}/include
)


set_target_properties(app_logic lloyd_relaxation_instrumented
PROPERTIES
    CMAKE_C_STANDARD 17
    CMAKE_C_STANDARD_REQUIRED ON
    CMAKE_C_EXTENSIONS OFF
)

set_target_properties(instr_tracer_pass lloyd_relaxation_instrumented
PROPERTIES
    CMAKE_CXX_STANDARD 23
    CMAKE_CXX_STANDARD_REQUIRED ON
    CMAKE_CXX_EXTENSIONS OFF
)
