# Matrix multiplication example
# Demonstrates basic tensor operations

tensor A[1024, 512] : f32
tensor B[512, 256] : f32
tensor C[1024, 256] : f32

# Matrix multiplication
C = matmul(A, B)

# Optional: Add bias
tensor bias[256] : f32
C = add(C, bias)