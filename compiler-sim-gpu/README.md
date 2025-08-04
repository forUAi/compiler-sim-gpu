# Compiler-Sim-GPU

A GPU compiler simulation framework demonstrating MLIR-style IR transformations, DWARF-inspired debugging, and GPU runtime simulation for tensor operations.

## Overview

This project simulates a compiler pipeline for GPU-targeted Domain-Specific Languages (DSLs), featuring:
- **MLIR-style IR**: Hierarchical intermediate representation with transformation passes
- **Debug Infrastructure**: DWARF-inspired symbol tracking and IR evolution tracing
- **GPU Runtime Simulation**: Mock GPU backend with performance metrics
- **Optimization Passes**: Loop unrolling, tensor fusion, and memory mapping

## Architecture

The compiler follows a multi-stage lowering pipeline:
```
DSL Source → Frontend Parser → High-Level IR → Optimization Passes → Low-Level IR → GPU Runtime
                                                     ↓
                                              Debug Trace Output
```

Key components:
- `IRNode`: Core IR representation with operation semantics
- `PassManager`: Orchestrates transformation pipeline
- `DebugInfo`: Tracks symbol mappings and IR evolution
- `MockGPU`: Simulates GPU dispatch and memory operations

## Building

```bash
mkdir build && cd build
cmake ..
make -j$(nproc)
```

## Usage

```bash
# Basic compilation
./compiler-sim examples/matmul.dsl

# With debug output
./compiler-sim examples/matmul.dsl --debug

# Emit IR after each pass
./compiler-sim examples/matmul.dsl --emit-ir

# Run GPU simulation
./compiler-sim examples/matmul.dsl --simulate-gpu
```

## Example DSL

```dsl
# matmul.dsl
tensor A[1024, 512] : f32
tensor B[512, 256] : f32
tensor C[1024, 256] : f32

C = matmul(A, B)
```

## Testing

```bash
cd build
ctest --verbose
```

## Documentation

- [Architecture Overview](docs/architecture.md)
- [IR Examples](docs/ir_examples.md)
- [Debugging Guide](docs/debugging.md)