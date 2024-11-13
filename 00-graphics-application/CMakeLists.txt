cmake_minimum_required(VERSION 3.21)

project(lloyd-relaxation
        LANGUAGES C)

if (${PROJECT_SOURCE_DIR} STREQUAL ${PROJECT_BINARY_DIR})
    message(FATAL_ERROR "In-source build is forbidden")
endif()

if (NOT ${CMAKE_C_COMPILER_ID} STREQUAL "Clang")
    message(FATAL_ERROR "Only clang may be used as C compiler")
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(SDL2 REQUIRED)

# graphics application executable
add_executable(lloyd_relaxation
    ./src/main.c
    ./src/graphics.c
    ./src/lloyd_relaxation.c
)

target_link_libraries(lloyd_relaxation PRIVATE SDL2)
target_include_directories(lloyd_relaxation PRIVATE ./include)

# graphics application object lib for use in the instrumenting pass (01-pass)
add_library(lloyd_relaxation_ir OBJECT ./src/lloyd_relaxation.c)

target_include_directories(lloyd_relaxation_ir PRIVATE ./include)
target_compile_options(lloyd_relaxation_ir PRIVATE ${CMAKE_C_FLAGS} -S -emit-llvm)


set_target_properties(lloyd_relaxation lloyd_relaxation_ir
PROPERTIES
    CMAKE_C_STANDARD 17
    CMAKE_C_STANDARD_REQUIRED ON
    CMAKE_C_EXTENSIONS OFF
)

install(TARGETS lloyd_relaxation lloyd_relaxation_ir
        DESTINATION ${PROJECT_SOURCE_DIR}/bin)
