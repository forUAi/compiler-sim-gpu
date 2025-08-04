#include "compiler_sim/SymbolTable.h"
#include <iostream>

namespace compiler_sim {

SymbolTable::SymbolTable() {
    // Start with global scope
    pushScope();
}

void SymbolTable::addSymbol(const Symbol& symbol) {
    if (scopes_.empty()) {
        pushScope();
    }
    scopes_.back().symbols[symbol.name] = symbol;
}

std::optional<Symbol> SymbolTable::lookupSymbol(const std::string& name) const {
    // Search from innermost to outermost scope
    for (auto it = scopes_.rbegin(); it != scopes_.rend(); ++it) {
        auto sym_it = it->symbols.find(name);
        if (sym_it != it->symbols.end()) {
            return sym_it->second;
        }
    }
    return std::nullopt;
}

bool SymbolTable::hasSymbol(const std::string& name) const {
    return lookupSymbol(name).has_value();
}

void SymbolTable::pushScope() {
    scopes_.emplace_back();
}

void SymbolTable::popScope() {
    if (!scopes_.empty()) {
        scopes_.pop_back();
    }
}

size_t SymbolTable::allocateMemory(const std::string& symbolName, size_t size) {
    size_t offset = totalMemory_;
    memoryOffsets_[symbolName] = offset;
    totalMemory_ += size;
    return offset;
}

std::vector<Symbol> SymbolTable::getAllSymbols() const {
    std::vector<Symbol> allSymbols;
    for (const auto& scope : scopes_) {
        for (const auto& [name, symbol] : scope.symbols) {
            allSymbols.push_back(symbol);
        }
    }
    return allSymbols;
}

void SymbolTable::dump() const {
    std::cout << "=== Symbol Table ===\n";
    for (size_t i = 0; i < scopes_.size(); ++i) {
        std::cout << "Scope " << i << ":\n";
        for (const auto& [name, symbol] : scopes_[i].symbols) {
            std::cout << "  " << name << ": ";
            std::cout << "type=" << static_cast<int>(symbol.type);
            std::cout << ", dtype=" << symbol.dtype;
            if (!symbol.shape.empty()) {
                std::cout << ", shape=[";
                for (size_t j = 0; j < symbol.shape.size(); ++j) {
                    if (j > 0) std::cout << ",";
                    std::cout << symbol.shape[j];
                }
                std::cout << "]";
            }
            std::cout << "\n";
        }
    }
    std::cout << "Total memory: " << totalMemory_ << " bytes\n";
}

} // namespace compiler_sim