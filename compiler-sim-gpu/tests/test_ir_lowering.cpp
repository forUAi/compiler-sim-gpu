#include <iostream>
#include <cassert>
#include "compiler_sim/IRNode.h"
#include "compiler_sim/PassManager.h"

using namespace compiler_sim;

void testBasicIRCreation() {
    std::cout << "Testing basic IR creation...\n";
    
    auto tensorA = createTensor("A", {512, 512}, "f32");
    assert(tensorA->getName() == "A");
    assert(tensorA->getType() == OpType::ALLOC);
    assert(tensorA->getAttribute<std::vector<int>>("shape")[0] == 512);
    
    auto tensorB = createTensor("B", {512, 512}, "f32");
    auto matmul = createMatmul("matmul_op", tensorA, tensorB);
    
    assert(matmul->getInputs().size() == 2);
    assert(matmul->getInputs()[0] == tensorA);
    assert(matmul->getInputs()[1] == tensorB);
    
    std::cout << "✓ Basic IR creation test passed\n";
}

void testIRCloning() {
    std::cout << "Testing IR cloning...\n";
    
    auto original = createTensor("original", {1024, 768}, "f16");
    original->setAttribute("custom_attr", 42);
    original->setDebugLocation(10, 5);
    
    auto cloned = original->clone();
    
    assert(cloned->getName() == original->getName());
    assert(cloned->getType() == original->getType());
    assert(cloned->getAttribute<int>("custom_attr") == 42);
    assert(cloned->getDebugLocation() == original->getDebugLocation());
    
    std::cout << "✓ IR cloning test passed\n";
}

void testPassManager() {
    std::cout << "Testing pass manager...\n";
    
    std::vector<std::shared_ptr<IRNode>> nodes;
    nodes.push_back(createTensor("A", {1024, 512}));
    nodes.push_back(createTensor("B", {512, 256}));
    
    PassManager pm(false, false);
    pm.addPass(createMemoryMapPass());
    
    size_t originalSize = nodes.size();
    pm.runPasses(nodes);
    
    // Verify memory offsets were added
    assert(nodes[0]->hasAttribute("memory_offset"));
    assert(nodes[1]->hasAttribute("memory_offset"));
    
    // Verify offsets are different
    int offset1 = nodes[0]->getAttribute<int>("memory_offset");
    int offset2 = nodes[1]->getAttribute<int>("memory_offset");
    assert(offset1 != offset2);
    
    std::cout << "✓ Pass manager test passed\n";
}

int main(int argc, char* argv[]) {
    if (argc > 1 && std::string(argv[1]) == "--test-ir") {
        std::cout << "Running IR lowering tests...\n\n";
        
        testBasicIRCreation();
        testIRCloning();
        testPassManager();
        
        std::cout << "\nAll IR lowering tests passed! ✓\n";
    }
    
    return 0;
}