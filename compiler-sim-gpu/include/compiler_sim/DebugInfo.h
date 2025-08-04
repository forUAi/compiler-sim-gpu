#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <fstream>
#include <json/json.h> // Assuming we use jsoncpp

namespace compiler_sim {

class IRNode;

struct DebugLocation {
    int line;
    int column;
    std::string file;
};

struct SymbolInfo {
    std::string name;
    std::string type;
    size_t memoryOffset;
    std::vector<int> shape;
    DebugLocation location;
};

struct PassTrace {
    std::string passName;
    std::string irBefore;
    std::string irAfter;
    std::vector<std::string> transformations;
    double executionTimeMs;
};

class DebugInfo {
public:
    DebugInfo();
    
    // Symbol tracking
    void addSymbol(const std::string& name, const SymbolInfo& info);
    const SymbolInfo* lookupSymbol(const std::string& name) const;
    
    // Pass tracing
    void beginPass(const std::string& passName);
    void endPass(const std::string& irAfter);
    void recordTransformation(const std::string& description);
    
    // Memory mapping
    void recordMemoryMapping(const std::string& tensor, 
                           size_t offset, 
                           size_t size);
    
    // IR evolution tracking
    void recordIRSnapshot(const std::string& stage, 
                         const std::vector<std::shared_ptr<IRNode>>& nodes);
    
    // Export debug trace
    void exportTrace(const std::string& filename) const;
    Json::Value toJson() const;

private:
    std::unordered_map<std::string, SymbolInfo> symbolTable_;
    std::vector<PassTrace> passTraces_;
    PassTrace* currentPass_ = nullptr;
    
    std::unordered_map<std::string, std::pair<size_t, size_t>> memoryMap_;
    std::vector<std::pair<std::string, std::string>> irSnapshots_;
    
    std::chrono::steady_clock::time_point passStartTime_;
};

} // namespace compiler_sim