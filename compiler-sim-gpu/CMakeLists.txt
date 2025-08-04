cmake_minimum_required(VERSION 3.14)
project(compiler-sim-gpu VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Source files
set(CORE_SOURCES
    src/IRNode.cpp
    src/PassManager.cpp
    src/DebugInfo.cpp
    src/SymbolTable.cpp
)

set(PASS_SOURCES
    passes/LoopUnrollingPass.cpp
    passes/TensorFusionPass.cpp
    passes/MemoryMapPass.cpp
)

set(RUNTIME_SOURCES
    runtimes/mock_gpu_runtime.cpp
)

# Main executable
add_executable(compiler-sim
    src/main.cpp
    ${CORE_SOURCES}
    ${PASS_SOURCES}
    ${RUNTIME_SOURCES}
)

# Enable testing
enable_testing()

# Test executable
add_executable(compiler-tests
    tests/test_ir_lowering.cpp
    tests/test_debug_hooks.cpp
    tests/test_codegen.cpp
    ${CORE_SOURCES}
    ${PASS_SOURCES}
    ${RUNTIME_SOURCES}
)

# Add tests
add_test(NAME IRLoweringTests COMMAND compiler-tests --test-ir)
add_test(NAME DebugHookTests COMMAND compiler-tests --test-debug)
add_test(NAME CodegenTests COMMAND compiler-tests --test-codegen)

# Set compiler flags
target_compile_options(compiler-sim PRIVATE
    -Wall -Wextra -Wpedantic
    $<$<CONFIG:Debug>:-g -O0>
    $<$<CONFIG:Release>:-O3>
)

target_compile_options(compiler-tests PRIVATE
    -Wall -Wextra -Wpedantic -g
)