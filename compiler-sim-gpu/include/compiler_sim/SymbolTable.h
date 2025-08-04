#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <optional>

namespace compiler_sim {

enum class SymbolType {
    TENSOR,
    SCALAR,
    FUNCTION,
    KERNEL
};

struct Symbol {
    std::string name;
    SymbolType type;
    std::vector<int> shape;  // For tensors
    std::string dtype;
    bool isConstant = false;
    size_t memorySize = 0;
    
    // Optional attributes
    std::unordered_map<std::string, std::string> attributes;
};

class SymbolTable {
public:
    SymbolTable();
    
    // Symbol management
    void addSymbol(const Symbol& symbol);
    std::optional<Symbol> lookupSymbol(const std::string& name) const;
    bool hasSymbol(const std::string& name) const;
    
    // Scope management
    void pushScope();
    void popScope();
    
    // Memory allocation tracking
    size_t allocateMemory(const std::string& symbolName, size_t size);
    size_t getTotalAllocatedMemory() const { return totalMemory_; }
    
    // Iteration
    std::vector<Symbol> getAllSymbols() const;
    
    // Debug
    void dump() const;

private:
    struct Scope {
        std::unordered_map<std::string, Symbol> symbols;
    };
    
    std::vector<Scope> scopes_;
    size_t totalMemory_ = 0;
    std::unordered_map<std::string, size_t> memoryOffsets_;
};

} // namespace compiler_sim