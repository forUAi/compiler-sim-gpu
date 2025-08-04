# Compiler Architecture

## Overview

The compiler-sim-gpu project implements a multi-stage compilation pipeline for GPU-targeted DSLs with comprehensive debugging support.

## Core Components

### IR Representation

The IR uses a hierarchical node-based structure inspired by MLIR:

```cpp
class IRNode {
    OpType type;           // Operation type (matmul, add, etc.)
    string name;           // Unique identifier
    vector<IRNode*> inputs;
    vector<IRNode*> outputs;
    map<string, Attribute> attributes;
    DebugLocation location;
};
```

### Pass Infrastructure

Transformation passes follow the visitor pattern:

```cpp
class Pass {
    virtual void run(vector<IRNode>& nodes, DebugInfo& debug) = 0;
};
```

Passes are orchestrated by the PassManager which handles:
- Pass scheduling and execution
- IR snapshot capture
- Debug trace generation
- Performance profiling

### Debug System

The debugging infrastructure provides:

1. **Symbol Tracking**: Maps high-level variables to memory locations
2. **IR Evolution**: Captures IR state before/after each pass
3. **Transformation Log**: Records all optimization decisions
4. **Memory Map**: Visualizes tensor memory layout

### GPU Runtime Simulation

The mock GPU runtime simulates:
- Memory allocation and management
- Kernel launch configuration
- Performance metrics (FLOPS, bandwidth)
- Execution timing

## Compilation Flow

1. **Frontend**: Parses DSL into high-level IR
2. **Analysis**: Builds symbol table and dependency graph
3. **Optimization**: Applies transformation passes
4. **Lowering**: Converts to GPU-executable form
5. **Runtime**: Simulates execution on GPU

## Extension Points

- Add new IR operations in `IRNode.h`
- Implement custom passes inheriting from `Pass`
- Extend debug hooks in `DebugInfo`
- Add new DSL constructs in the parser