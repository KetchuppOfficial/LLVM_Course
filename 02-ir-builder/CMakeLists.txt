set(APP_DIR ${PROJECT_SOURCE_DIR}/00-graphics-application)

add_executable(legacy_ir_builder
    ./src/builder.cpp
    ${APP_DIR}/src/legacy/graphics.c
)
target_include_directories(legacy_ir_builder
PRIVATE
    ${LLVM_INCLUDE_DIRS}
    ${APP_DIR}/include/legacy
)
target_link_libraries(legacy_ir_builder
PRIVATE
    LLVM
    SDL2
    m
)

install(TARGETS legacy_ir_builder)
