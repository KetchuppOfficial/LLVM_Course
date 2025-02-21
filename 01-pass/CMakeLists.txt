# shared library with instrumenting LLVM pass
add_library(instr_tracer_pass SHARED ./src/pass.cpp)
target_include_directories(instr_tracer_pass PRIVATE ${LLVM_INCLUDE_DIRS})

# application logic compiled with the instrumenting pass
set(APP_DIR ${PROJECT_SOURCE_DIR}/00-graphics-application)
add_library(app_logic_instrumented OBJECT ${APP_DIR}/src/lloyd_relaxation.c)
add_dependencies(app_logic_instrumented instr_tracer_pass)
target_include_directories(app_logic_instrumented PUBLIC ${APP_DIR}/include)
target_compile_options(app_logic_instrumented PRIVATE -fpass-plugin=$<TARGET_FILE:instr_tracer_pass>)

# final instrumented graphics application
add_executable(lloyd_relaxation_instrumented
    ${APP_DIR}/src/main.c
    ./src/logger.cpp
)
target_link_libraries(lloyd_relaxation_instrumented
PRIVATE
    LLVM
    app_graphics
    app_logic_instrumented
)
target_include_directories(lloyd_relaxation_instrumented PRIVATE ${LLVM_INCLUDE_DIRS})

install(TARGETS lloyd_relaxation_instrumented)
