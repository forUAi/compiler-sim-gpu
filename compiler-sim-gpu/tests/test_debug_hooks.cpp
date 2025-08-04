#include <iostream>
#include <cassert>
#include <fstream>
#include "compiler_sim/DebugInfo.h"
#include "compiler_sim/IRNode.h"

using namespace compiler_sim;

void testSymbolTable() {
    std::cout << "Testing symbol table...\n";
    
    DebugInfo debug;
    
    SymbolInfo info;
    info.name = "tensor_A";
    info.type = "tensor";
    info.memoryOffset = 0;
    info.shape = {1024, 512};
    info.location = {3, 1, "test.dsl"};
    
    debug.addSymbol("tensor_A", info);
    
    auto* retrieved = debug.lookupSymbol("tensor_A");
    assert(retrieved != nullptr);
    assert(retrieved->name == "tensor_A");
    assert(retrieved->memoryOffset == 0);
    assert(retrieved->shape[0] == 1024);
    
    std::cout << "✓ Symbol table test passed\n";
}

void testPassTracing() {
    std::cout << "Testing pass tracing...\n";
    
    DebugInfo debug;
    
    debug.beginPass("TestPass");
    debug.recordTransformation("Applied optimization X");
    debug.recordTransformation("Removed redundant operation Y");
    debug.endPass("IR after TestPass");
    
    // Export and verify
    debug.exportTrace("test_trace.json");
    
    std::ifstream file("test_trace.json");
    assert(file.good());
    file.close();
    
    std::cout << "✓ Pass tracing test passed\n";
}

void testMemoryMapping() {
    std::cout << "Testing memory mapping...\n";
    
    DebugInfo debug;
    
    debug.recordMemoryMapping("tensorA", 0, 4194304);      // 4MB
    debug.recordMemoryMapping("tensorB", 4194304, 2097152); // 2MB
    debug.recordMemoryMapping("tensorC", 6291456, 4194304); // 4MB
    
    auto json = debug.toJson();
    assert(json["memory_map"]["tensorA"]["offset"] == 0);
    assert(json["memory_map"]["tensorB"]["offset"] == 4194304);
    assert(json["memory_map"]["tensorC"]["size"] == 4194304);
    
    std::cout << "✓ Memory mapping test passed\n";
}

int main(int argc, char* argv[]) {
    if (argc > 1 && std::string(argv[1]) == "--test-debug") {
        std::cout << "Running debug hook tests...\n\n";
        
        testSymbolTable();
        testPassTracing();
        testMemoryMapping();
        
        std::cout << "\nAll debug hook tests passed! ✓\n";
    }
    
    return 0;
}