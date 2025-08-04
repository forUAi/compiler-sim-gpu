#include "compiler_sim/PassManager.h"
#include "compiler_sim/IRNode.h"
#include <unordered_map>

namespace compiler_sim {

class MemoryMapPass : public Pass {
public:
    std::string getName() const override {
        return "MemoryMapPass";
    }
    
    void run(std::vector<std::shared_ptr<IRNode>>& nodes,
            DebugInfo& debugInfo) override {
        
        size_t currentOffset = 0;
        const size_t alignment = 256; // GPU memory alignment
        
        std::unordered_map<std::string, size_t> memoryMap;
        
        for (auto& node : nodes) {
            if (node->getType() == OpType::ALLOC) {
                // Calculate memory size
                auto shape = node->getAttribute<std::vector<int>>("shape");
                auto dtype = node->getAttribute<std::string>("dtype");
                
                size_t elementSize = 4; // f32 by default
                if (dtype == "f16") elementSize = 2;
                else if (dtype == "f64") elementSize = 8;
                
                size_t totalElements = 1;
                for (int dim : shape) {
                    totalElements *= dim;
                }
                
                size_t memorySize = totalElements * elementSize;
                
                // Align to GPU requirements
                if (currentOffset % alignment != 0) {
                    currentOffset = ((currentOffset / alignment) + 1) * alignment;
                }
                
                // Record mapping
                memoryMap[node->getName()] = currentOffset;
                node->setAttribute("memory_offset", static_cast<int>(currentOffset));
                node->setAttribute("memory_size", static_cast<int>(memorySize));
                
                debugInfo.recordMemoryMapping(
                    node->getName(),
                    currentOffset,
                    memorySize
                );
                
                debugInfo.recordTransformation(
                    "Mapped tensor " + node->getName() + 
                    " to offset " + std::to_string(currentOffset) +
                    " (size: " + std::to_string(memorySize) + " bytes)"
                );
                
                currentOffset += memorySize;
            }
        }
        
        debugInfo.recordTransformation(
            "Total memory allocated: " + std::to_string(currentOffset) + " bytes"
        );
    }
};

std::unique_ptr<Pass> createMemoryMapPass() {
    return std::make_unique<MemoryMapPass>();
}

} // namespace compiler_sim