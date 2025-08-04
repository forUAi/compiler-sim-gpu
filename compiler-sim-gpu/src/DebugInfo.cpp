#include "compiler_sim/DebugInfo.h"
#include "compiler_sim/IRNode.h"
#include <chrono>
#include <fstream>
#include <iomanip>

namespace compiler_sim {

DebugInfo::DebugInfo() {}

void DebugInfo::addSymbol(const std::string& name, const SymbolInfo& info) {
    symbolTable_[name] = info;
}

const SymbolInfo* DebugInfo::lookupSymbol(const std::string& name) const {
    auto it = symbolTable_.find(name);
    return it != symbolTable_.end() ? &it->second : nullptr;
}

void DebugInfo::beginPass(const std::string& passName) {
    passTraces_.emplace_back();
    currentPass_ = &passTraces_.back();
    currentPass_->passName = passName;
    passStartTime_ = std::chrono::steady_clock::now();
}

void DebugInfo::endPass(const std::string& irAfter) {
    if (currentPass_) {
        currentPass_->irAfter = irAfter;
        auto endTime = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
            endTime - passStartTime_
        );
        currentPass_->executionTimeMs = duration.count() / 1000.0;
        currentPass_ = nullptr;
    }
}

void DebugInfo::recordTransformation(const std::string& description) {
    if (currentPass_) {
        currentPass_->transformations.push_back(description);
    }
}

void DebugInfo::recordMemoryMapping(const std::string& tensor,
                                   size_t offset,
                                   size_t size) {
    memoryMap_[tensor] = {offset, size};
}

void DebugInfo::recordIRSnapshot(const std::string& stage,
                                const std::vector<std::shared_ptr<IRNode>>& nodes) {
    std::string snapshot;
    for (const auto& node : nodes) {
        snapshot += node->toString() + "\n";
    }
    irSnapshots_.emplace_back(stage, snapshot);
}

void DebugInfo::exportTrace(const std::string& filename) const {
    Json::Value root;
    root["trace"] = toJson();
    
    Json::StreamWriterBuilder builder;
    builder["indentation"] = "  ";
    std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
    
    std::ofstream file(filename);
    writer->write(root, &file);
}

Json::Value DebugInfo::toJson() const {
    Json::Value root;
    
    // Symbol table
    Json::Value symbols(Json::objectValue);
    for (const auto& [name, info] : symbolTable_) {
        Json::Value sym;
        sym["type"] = info.type;
        sym["memory_offset"] = static_cast<Json::UInt64>(info.memoryOffset);
        sym["shape"] = Json::arrayValue;
        for (int dim : info.shape) {
            sym["shape"].append(dim);
        }
        sym["location"]["line"] = info.location.line;
        sym["location"]["column"] = info.location.column;
        sym["location"]["file"] = info.location.file;
        symbols[name] = sym;
    }
    root["symbols"] = symbols;
    
    // Pass traces
    Json::Value passes(Json::arrayValue);
    for (const auto& trace : passTraces_) {
        Json::Value pass;
        pass["name"] = trace.passName;
        pass["execution_time_ms"] = trace.executionTimeMs;
        pass["transformations"] = Json::arrayValue;
        for (const auto& transform : trace.transformations) {
            pass["transformations"].append(transform);
        }
        passes.append(pass);
    }
    root["passes"] = passes;
    
    // Memory map
    Json::Value memory(Json::objectValue);
    for (const auto& [tensor, mapping] : memoryMap_) {
        Json::Value mem;
        mem["offset"] = static_cast<Json::UInt64>(mapping.first);
        mem["size"] = static_cast<Json::UInt64>(mapping.second);
        memory[tensor] = mem;
    }
    root["memory_map"] = memory;
    
    return root;
}

} // namespace compiler_sim