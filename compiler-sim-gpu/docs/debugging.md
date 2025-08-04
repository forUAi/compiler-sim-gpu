# Debugging Guide

## Debug Trace Format

The compiler generates comprehensive debug traces in JSON format:

```json
{
  "trace": {
    "symbols": {
      "A": {
        "type": "tensor",
        "memory_offset": 0,
        "shape": [1024, 512],
        "location": {
          "line": 3,
          "column": 1,
          "file": "matmul.dsl"
        }
      }
    },
    "passes": [
      {
        "name": "LoopUnrollingPass",
        "execution_time_ms": 2.341,
        "transformations": [
          "Unrolled loop_0 by factor 4",
          "Eliminated 3 loop overhead instructions"
        ]
      }
    ],
    "memory_map": {
      "A": {"offset": 0, "size": 2097152},
      "B": {"offset": 2097152, "size": 1048576}
    }
  }
}
```

## Using Debug Flags

### --debug
Enables comprehensive debugging output including:
- Symbol table dumps
- Pass execution traces
- Memory allocation logs
- Performance metrics

### --emit-ir
Prints IR after each transformation pass:
```bash
./compiler-sim examples/matmul.dsl --emit-ir

=== Before LoopUnrollingPass ===
%loop_0 = loop {start = 0, end = 1024, step = 1}
...

=== After LoopUnrollingPass ===
%loop_0_unroll_0 = block {iteration = 0}
%loop_0_unroll_1 = block {iteration = 1}
...
```

### --simulate-gpu
Runs the mock GPU runtime showing:
- Kernel launch configurations
- Memory allocation patterns
- Simulated performance metrics

## Debugging Workflow

1. **Initial Compilation**: Run with `--debug` to identify issues
2. **IR Analysis**: Use `--emit-ir` to track transformations
3. **Memory Debugging**: Check trace.json for allocation patterns
4. **Performance**: Use `--simulate-gpu` for runtime analysis

## Common Issues

### Out of Memory
Check memory_map in trace.json for:
- Overlapping allocations
- Excessive fragmentation
- Missing deallocations

### Incorrect Results
Verify IR transformations:
- Loop bounds after unrolling
- Fusion correctness
- Memory access patterns

### Performance Issues
Analyze with `--simulate-gpu`:
- Kernel launch efficiency
- Memory bandwidth utilization
- Computational intensity