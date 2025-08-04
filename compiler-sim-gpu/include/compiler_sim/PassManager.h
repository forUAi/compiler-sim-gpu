#pragma once

#include <memory>
#include <vector>
#include <functional>
#include <string>
#include "IRNode.h"
#include "DebugInfo.h"

namespace compiler_sim {

class Pass {
public:
    virtual ~Pass() = default;
    virtual std::string getName() const = 0;
    virtual void run(std::vector<std::shared_ptr<IRNode>>& nodes,
                    DebugInfo& debugInfo) = 0;
};

class PassManager {
public:
    PassManager(bool emitIR = false, bool debug = false);
    
    // Pass registration
    void addPass(std::unique_ptr<Pass> pass);
    
    // Run all passes
    void runPasses(std::vector<std::shared_ptr<IRNode>>& nodes);
    
    // Debug output control
    void setEmitIR(bool emit) { emitIR_ = emit; }
    void setDebugMode(bool debug) { debug_ = debug; }
    
    // Get debug info
    const DebugInfo& getDebugInfo() const { return debugInfo_; }

private:
    std::vector<std::unique_ptr<Pass>> passes_;
    DebugInfo debugInfo_;
    bool emitIR_;
    bool debug_;
    
    void emitIRSnapshot(const std::string& passName,
                       const std::vector<std::shared_ptr<IRNode>>& nodes);
};

// Standard pass implementations
std::unique_ptr<Pass> createLoopUnrollingPass(int unrollFactor = 4);
std::unique_ptr<Pass> createTensorFusionPass();
std::unique_ptr<Pass> createMemoryMapPass();

} // namespace compiler_sim