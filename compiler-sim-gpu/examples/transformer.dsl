# Simplified transformer layer
# Demonstrates more complex operations

# Input embeddings
tensor input[32, 512, 768] : f32  # [batch, seq_len, hidden]

# Self-attention weights
tensor Wq[768, 768] : f32
tensor Wk[768, 768] : f32
tensor Wv[768, 768] : f32

# Compute Q, K, V
tensor Q[32, 512, 768] : f32
tensor K[32, 512, 768] : f32
tensor V[32, 512, 768] : f32

Q = matmul(input, Wq)
K = matmul(input, Wk)
V = matmul(input, Wv)

# Attention scores
tensor scores[32, 512, 512] : f32
scores = matmul(Q, transpose(K))
scores = scale(scores, 0.125)  # 1/sqrt(64)

# Softmax (simplified as single op)
tensor attention[32, 512, 512] : f32
attention = softmax(scores)

# Apply attention
tensor output[32, 512, 768] : f32
output = matmul(attention, V)