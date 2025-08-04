#include "compiler_sim/PassManager.h"
#include "compiler_sim/IRNode.h"
#include <algorithm>

namespace compiler_sim {

class LoopUnrollingPass : public Pass {
public:
    LoopUnrollingPass(int unrollFactor) : unrollFactor_(unrollFactor) {}
    
    std::string getName() const override {
        return "LoopUnrollingPass";
    }
    
    void run(std::vector<std::shared_ptr<IRNode>>& nodes,
            DebugInfo& debugInfo) override {
        
        std::vector<std::shared_ptr<IRNode>> newNodes;
        
        for (auto& node : nodes) {
            if (node->getType() == OpType::LOOP) {
                // Simulate loop unrolling
                debugInfo.recordTransformation(
                    "Unrolling loop " + node->getName() + 
                    " by factor " + std::to_string(unrollFactor_));
                
                // Get loop bounds
                int start = node->getAttribute<int>("start");
                int end = node->getAttribute<int>("end");
                int step = node->getAttribute<int>("step");
                
                // Create unrolled iterations
                for (int i = start; i < end; i += step * unrollFactor_) {
                    for (int j = 0; j < unrollFactor_ && i + j * step < end; j++) {
                        auto unrolled = std::make_shared<IRNode>(
                            OpType::BLOCK, 
                            node->getName() + "_unroll_" + std::to_string(i + j * step)
                        );
                        unrolled->setAttribute("iteration", i + j * step);
                        newNodes.push_back(unrolled);
                    }
                }
            } else {
                newNodes.push_back(node);
            }
        }
        
        nodes = std::move(newNodes);
    }
    
private:
    int unrollFactor_;
};

std::unique_ptr<Pass> createLoopUnrollingPass(int unrollFactor) {
    return std::make_unique<LoopUnrollingPass>(unrollFactor);
}

} // namespace compiler_sim