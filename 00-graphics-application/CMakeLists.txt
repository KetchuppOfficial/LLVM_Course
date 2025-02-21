# app graphics
add_library(app_graphics OBJECT ./src/graphics.c)
target_include_directories(app_graphics PUBLIC ./include)
target_link_libraries(app_graphics PUBLIC SDL2)

# graphics application executable
add_executable(lloyd_relaxation
    ./src/main.c
    ./src/lloyd_relaxation.c
)
target_link_libraries(lloyd_relaxation PRIVATE app_graphics)

# LLVM IR generation
add_library(lloyd_relaxation_ir OBJECT ./src/lloyd_relaxation.c)
target_include_directories(lloyd_relaxation_ir PRIVATE ./include)
target_compile_options(lloyd_relaxation_ir PRIVATE ${CMAKE_C_FLAGS} -S -emit-llvm)

install(FILES $<TARGET_OBJECTS:lloyd_relaxation_ir>
        TYPE BIN
        RENAME lloyd_relaxation.ll)

install(TARGETS lloyd_relaxation)
