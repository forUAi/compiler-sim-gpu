#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <variant>

namespace compiler_sim {

enum class OpType {
    MATMUL,
    ADD,
    MUL,
    LOAD,
    STORE,
    ALLOC,
    LOOP,
    BLOCK
};

using AttributeValue = std::variant<int, float, std::string, std::vector<int>>;

class IRNode {
public:
    IRNode(OpType type, const std::string& name);
    
    // Builder pattern for node construction
    IRNode& addInput(std::shared_ptr<IRNode> input);
    IRNode& addOutput(std::shared_ptr<IRNode> output);
    IRNode& setAttribute(const std::string& key, AttributeValue value);
    
    // Accessors
    OpType getType() const { return type_; }
    const std::string& getName() const { return name_; }
    const std::vector<std::shared_ptr<IRNode>>& getInputs() const { return inputs_; }
    const std::vector<std::shared_ptr<IRNode>>& getOutputs() const { return outputs_; }
    
    // Debug information
    void setDebugLocation(int line, int col);
    std::pair<int, int> getDebugLocation() const { return {debug_line_, debug_col_}; }
    
    // Clone for transformation passes
    std::shared_ptr<IRNode> clone() const;
    
    // Pretty printing
    std::string toString(int indent = 0) const;
    
    // Attribute access
    template<typename T>
    T getAttribute(const std::string& key) const {
        auto it = attributes_.find(key);
        if (it != attributes_.end()) {
            return std::get<T>(it->second);
        }
        throw std::runtime_error("Attribute not found: " + key);
    }
    
    bool hasAttribute(const std::string& key) const {
        return attributes_.find(key) != attributes_.end();
    }

private:
    OpType type_;
    std::string name_;
    std::vector<std::shared_ptr<IRNode>> inputs_;
    std::vector<std::shared_ptr<IRNode>> outputs_;
    std::unordered_map<std::string, AttributeValue> attributes_;
    int debug_line_ = -1;
    int debug_col_ = -1;
};

// Helper factory functions
std::shared_ptr<IRNode> createMatmul(const std::string& name,
                                     std::shared_ptr<IRNode> a,
                                     std::shared_ptr<IRNode> b);

std::shared_ptr<IRNode> createTensor(const std::string& name,
                                     const std::vector<int>& shape,
                                     const std::string& dtype = "f32");

} // namespace compiler_sim