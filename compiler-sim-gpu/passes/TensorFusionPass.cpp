#include "compiler_sim/PassManager.h"
#include "compiler_sim/IRNode.h"
#include <unordered_set>

namespace compiler_sim {

class TensorFusionPass : public Pass {
public:
    std::string getName() const override {
        return "TensorFusionPass";
    }
    
    void run(std::vector<std::shared_ptr<IRNode>>& nodes,
            DebugInfo& debugInfo) override {
        
        std::vector<std::shared_ptr<IRNode>> fusedNodes;
        std::unordered_set<size_t> fusedIndices;
        
        // Look for fusable patterns
        for (size_t i = 0; i < nodes.size(); i++) {
            if (fusedIndices.count(i)) continue;
            
            auto& node = nodes[i];
            
            // Pattern: matmul followed by add (common in neural networks)
            if (node->getType() == OpType::MATMUL && i + 1 < nodes.size()) {
                auto& next = nodes[i + 1];
                
                if (next->getType() == OpType::ADD &&
                    !next->getInputs().empty() &&
                    next->getInputs()[0] == node) {
                    
                    // Create fused operation
                    auto fused = std::make_shared<IRNode>(
                        OpType::MATMUL,
                        node->getName() + "_fused_add"
                    );
                    
                    // Copy inputs from matmul
                    for (auto& input : node->getInputs()) {
                        fused->addInput(input);
                    }
                    
                    // Add bias input from add operation
                    if (next->getInputs().size() > 1) {
                        fused->addInput(next->getInputs()[1]);
                    }
                    
                    // Set fusion attribute
                    fused->setAttribute("fused_ops", std::string("matmul_add"));
                    
                    // Copy outputs from add
                    for (auto& output : next->getOutputs()) {
                        fused->addOutput(output);
                    }
                    
                    fusedNodes.push_back(fused);
                    fusedIndices.insert(i);
                    fusedIndices.insert(i + 1);
                    
                    debugInfo.recordTransformation(
                        "Fused " + node->getName() + " and " + 
                        next->getName() + " into " + fused->getName()
                    );
                    
                    i++; // Skip next node
                    continue;
                }
            }
            
            if (!fusedIndices.count(i)) {
                fusedNodes.push_back(node);
            }
        }
        
        nodes = std::move(fusedNodes);
    }
};

std::unique_ptr<Pass> createTensorFusionPass() {
    return std::make_unique<TensorFusionPass>();
}

} // namespace compiler_sim