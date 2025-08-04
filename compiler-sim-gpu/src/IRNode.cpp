#include "compiler_sim/IRNode.h"
#include <sstream>
#include <iomanip>

namespace compiler_sim {

IRNode::IRNode(OpType type, const std::string& name)
    : type_(type), name_(name) {}

IRNode& IRNode::addInput(std::shared_ptr<IRNode> input) {
    inputs_.push_back(input);
    return *this;
}

IRNode& IRNode::addOutput(std::shared_ptr<IRNode> output) {
    outputs_.push_back(output);
    return *this;
}

IRNode& IRNode::setAttribute(const std::string& key, AttributeValue value) {
    attributes_[key] = value;
    return *this;
}

void IRNode::setDebugLocation(int line, int col) {
    debug_line_ = line;
    debug_col_ = col;
}

std::shared_ptr<IRNode> IRNode::clone() const {
    auto cloned = std::make_shared<IRNode>(type_, name_);
    cloned->attributes_ = attributes_;
    cloned->debug_line_ = debug_line_;
    cloned->debug_col_ = debug_col_;
    
    // Note: This creates a shallow copy of inputs/outputs
    // Deep cloning would require a more complex graph traversal
    cloned->inputs_ = inputs_;
    cloned->outputs_ = outputs_;
    
    return cloned;
}

std::string IRNode::toString(int indent) const {
    std::stringstream ss;
    std::string ind(indent * 2, ' ');
    
    ss << ind << "%" << name_ << " = ";
    
    switch (type_) {
        case OpType::MATMUL:
            ss << "matmul";
            break;
        case OpType::ADD:
            ss << "add";
            break;
        case OpType::MUL:
            ss << "mul";
            break;
        case OpType::LOAD:
            ss << "load";
            break;
        case OpType::STORE:
            ss << "store";
            break;
        case OpType::ALLOC:
            ss << "alloc";
            break;
        case OpType::LOOP:
            ss << "loop";
            break;
        case OpType::BLOCK:
            ss << "block";
            break;
    }
    
    // Print operands
    if (!inputs_.empty()) {
        ss << "(";
        for (size_t i = 0; i < inputs_.size(); ++i) {
            if (i > 0) ss << ", ";
            ss << "%" << inputs_[i]->getName();
        }
        ss << ")";
    }
    
    // Print attributes
    if (!attributes_.empty()) {
        ss << " {";
        bool first = true;
        for (const auto& [key, value] : attributes_) {
            if (!first) ss << ", ";
            first = false;
            ss << key << " = ";
            
            std::visit([&ss](const auto& v) {
                using T = std::decay_t<decltype(v)>;
                if constexpr (std::is_same_v<T, std::vector<int>>) {
                    ss << "[";
                    for (size_t i = 0; i < v.size(); ++i) {
                        if (i > 0) ss << ", ";
                        ss << v[i];
                    }
                    ss << "]";
                } else {
                    ss << v;
                }
            }, value);
        }
        ss << "}";
    }
    
    // Debug location
    if (debug_line_ >= 0) {
        ss << " !loc(" << debug_line_ << ":" << debug_col_ << ")";
    }
    
    return ss.str();
}

// Helper factory functions
std::shared_ptr<IRNode> createMatmul(const std::string& name,
                                     std::shared_ptr<IRNode> a,
                                     std::shared_ptr<IRNode> b) {
    auto node = std::make_shared<IRNode>(OpType::MATMUL, name);
    node->addInput(a).addInput(b);
    return node;
}

std::shared_ptr<IRNode> createTensor(const std::string& name,
                                     const std::vector<int>& shape,
                                     const std::string& dtype) {
    auto node = std::make_shared<IRNode>(OpType::ALLOC, name);
    node->setAttribute("shape", shape);
    node->setAttribute("dtype", dtype);
    
    // Calculate size
    int size = 1;
    for (int dim : shape) {
        size *= dim;
    }
    node->setAttribute("size", size);
    
    return node;
}

} // namespace compiler_sim