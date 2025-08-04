#include "compiler_sim/PassManager.h"
#include <iostream>
#include <chrono>

namespace compiler_sim {

PassManager::PassManager(bool emitIR, bool debug)
    : emitIR_(emitIR), debug_(debug) {}

void PassManager::addPass(std::unique_ptr<Pass> pass) {
    passes_.push_back(std::move(pass));
}

void PassManager::runPasses(std::vector<std::shared_ptr<IRNode>>& nodes) {
    for (auto& pass : passes_) {
        if (debug_) {
            std::cout << "Running pass: " << pass->getName() << "\n";
        }
        
        debugInfo_.beginPass(pass->getName());
        
        // Capture IR before pass
        if (emitIR_) {
            emitIRSnapshot("Before " + pass->getName(), nodes);
        }
        
        // Run the pass
        pass->run(nodes, debugInfo_);
        
        // Capture IR after pass
        if (emitIR_) {
            emitIRSnapshot("After " + pass->getName(), nodes);
        }
        
        // Record IR snapshot for debug trace
        std::string irAfter;
        for (const auto& node : nodes) {
            irAfter += node->toString() + "\n";
        }
        debugInfo_.endPass(irAfter);
    }
}

void PassManager::emitIRSnapshot(const std::string& passName,
                                const std::vector<std::shared_ptr<IRNode>>& nodes) {
    std::cout << "\n=== " << passName << " ===\n";
    for (const auto& node : nodes) {
        std::cout << node->toString() << "\n";
    }
    std::cout << "\n";
}

} // namespace compiler_sim