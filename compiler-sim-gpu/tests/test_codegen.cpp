#include <iostream>
#include <cassert>
#include "compiler_sim/IRNode.h"
#include "compiler_sim/PassManager.h"

using namespace compiler_sim;

void testLoopUnrolling() {
    std::cout << "Testing loop unrolling pass...\n";
    
    std::vector<std::shared_ptr<IRNode>> nodes;
    
    // Create a loop node
    auto loop = std::make_shared<IRNode>(OpType::LOOP, "main_loop");
    loop->setAttribute("start", 0);
    loop->setAttribute("end", 100);
    loop->setAttribute("step", 1);
    
    nodes.push_back(loop);
    
    // Apply loop unrolling
    PassManager pm;
    pm.addPass(createLoopUnrollingPass(4));
    
    size_t originalSize = nodes.size();
    pm.runPasses(nodes);
    
    // Should have multiple unrolled blocks now
    assert(nodes.size() > originalSize);
    
    std::cout << "✓ Loop unrolling test passed\n";
}

void testTensorFusion() {
    std::cout << "Testing tensor fusion pass...\n";
    
    std::vector<std::shared_ptr<IRNode>> nodes;
    
    // Create a matmul followed by add (common pattern)
    auto tensorA = createTensor("A", {1024, 512});
    auto tensorB = createTensor("B", {512, 256});
    auto tensorC = createTensor("C", {1024, 256});
    auto bias = createTensor("bias", {256});
    
    auto matmul = createMatmul("matmul_op", tensorA, tensorB);
    matmul->addOutput(tensorC);
    
    auto add = std::make_shared<IRNode>(OpType::ADD, "add_op");
    add->addInput(matmul);
    add->addInput(bias);
    
    nodes.push_back(tensorA);
    nodes.push_back(tensorB);
    nodes.push_back(tensorC);
    nodes.push_back(bias);
    nodes.push_back(matmul);
    nodes.push_back(add);
    
    // Apply fusion pass
    PassManager pm;
    pm.addPass(createTensorFusionPass());
    
    size_t originalSize = nodes.size();
    pm.runPasses(nodes);
    
    // Should have fewer nodes after fusion
    assert(nodes.size() < originalSize);
    
    // Check for fused operation
    bool foundFused = false;
    for (const auto& node : nodes) {
        if (node->hasAttribute("fused_ops")) {
            foundFused = true;
            break;
        }
    }
    assert(foundFused);
    
    std::cout << "✓ Tensor fusion test passed\n";
}

void testMemoryAllocation() {
    std::cout << "Testing memory allocation...\n";
    
    std::vector<std::shared_ptr<IRNode>> nodes;
    
    // Create tensors of different sizes
    nodes.push_back(createTensor("small", {32, 32}, "f32"));    // 4KB
    nodes.push_back(createTensor("medium", {512, 512}, "f32")); // 1MB
    nodes.push_back(createTensor("large", {2048, 2048}, "f32")); // 16MB
    
    // Apply memory mapping
    PassManager pm;
    pm.addPass(createMemoryMapPass());
    pm.runPasses(nodes);
    
    // Verify all tensors have memory offsets
    for (const auto& node : nodes) {
        assert(node->hasAttribute("memory_offset"));
        assert(node->hasAttribute("memory_size"));
    }
    
    // Verify no overlapping memory regions
    std::vector<std::pair<int, int>> regions;
    for (const auto& node : nodes) {
        int offset = node->getAttribute<int>("memory_offset");
        int size = node->getAttribute<int>("memory_size");
        regions.push_back({offset, offset + size});
    }
    
    // Sort by offset
    std::sort(regions.begin(), regions.end());
    
    // Check for overlaps
    for (size_t i = 1; i < regions.size(); i++) {
        assert(regions[i].first >= regions[i-1].second);
    }
    
    std::cout << "✓ Memory allocation test passed\n";
}

int main(int argc, char* argv[]) {
    if (argc > 1 && std::string(argv[1]) == "--test-codegen") {
        std::cout << "Running codegen tests...\n\n";
        
        testLoopUnrolling();
        testTensorFusion();
        testMemoryAllocation();
        
        std::cout << "\nAll codegen tests passed! ✓\n";
    }
    
    return 0;
}