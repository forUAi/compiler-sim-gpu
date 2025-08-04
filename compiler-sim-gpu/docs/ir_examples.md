# IR Examples

## Basic Tensor Operation

DSL Input:
```dsl
tensor A[512, 512] : f32
tensor B[512, 512] : f32
tensor C[512, 512] : f32
C = add(A, B)
```

Initial IR:
```
%A = alloc {shape = [512, 512], dtype = "f32", size = 262144}
%B = alloc {shape = [512, 512], dtype = "f32", size = 262144}
%C = alloc {shape = [512, 512], dtype = "f32", size = 262144}
%add_op = add(%A, %B) {shape = [512, 512]}
%store_op = store(%add_op, %C)
```

After Memory Mapping:
```
%A = alloc {shape = [512, 512], dtype = "f32", size = 262144, memory_offset = 0}
%B = alloc {shape = [512, 512], dtype = "f32", size = 262144, memory_offset = 1048576}
%C = alloc {shape = [512, 512], dtype = "f32", size = 262144, memory_offset = 2097152}
%add_op = add(%A, %B) {shape = [512, 512]}
%store_op = store(%add_op, %C)
```

## Loop Unrolling Example

Before:
```
%loop_0 = loop {start = 0, end = 1024, step = 1}
  %load = load(%A, %i)
  %compute = mul(%load, 2.0)
  %store = store(%compute, %B, %i)
%end_loop
```

After (unroll factor = 4):
```
%loop_0 = loop {start = 0, end = 1024, step = 4}
  %load_0 = load(%A, %i)
  %compute_0 = mul(%load_0, 2.0)
  %store_0 = store(%compute_0, %B, %i)
  
  %load_1 = load(%A, %i+1)
  %compute_1 = mul(%load_1, 2.0)
  %store_1 = store(%compute_1, %B, %i+1)
  
  %load_2 = load(%A, %i+2)
  %compute_2 = mul(%load_2, 2.0)
  %store_2 = store(%compute_2, %B, %i+2)
  
  %load_3 = load(%A, %i+3)
  %compute_3 = mul(%load_3, 2.0)
  %store_3 = store(%compute_3, %B, %i+3)
%end_loop
```

## Tensor Fusion Example

Before:
```
%matmul_result = matmul(%A, %B) {M = 1024, N = 512, K = 768}
%bias_add = add(%matmul_result, %bias) {broadcast = true}
%activation = relu(%bias_add)
```

After:
```
%fused_matmul_bias_relu = matmul_bias_relu(%A, %B, %bias) {
    M = 1024, N = 512, K = 768,
    fused_ops = ["matmul", "bias_add", "relu"],
    kernel = "gemm_bias_relu_kernel"
}
```