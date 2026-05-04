#include "ASTNode.h"
#include <iostream>
#include <sstream>

namespace TaskScript {

static int globalNodeCounter = 0;

void ASTNode::resetDotCounter() {
    globalNodeCounter = 0;
}

ASTNode::ASTNode(const std::string& productionName)
    : name(productionName), 
      token(TokenType::DESCONOCIDO, "", 0, 0), 
      isTerminal(false) {}

ASTNode::ASTNode(const Token& token)
    : name(token.typeToString()), 
      token(token), 
      isTerminal(true) {}

ASTNode::ASTNode(const std::string& name, const Token& token)
    : name(name), 
      token(token), 
      isTerminal(true) {}

ASTNode::ASTNode(ASTNode&& other) noexcept
    : name(std::move(other.name)),
      token(std::move(other.token)),
      isTerminal(other.isTerminal),
      children(std::move(other.children)) {}

ASTNode& ASTNode::operator=(ASTNode&& other) noexcept {
    if (this != &other) {
        name = std::move(other.name);
        token = std::move(other.token);
        isTerminal = other.isTerminal;
        children = std::move(other.children);
    }
    return *this;
}

const std::string& ASTNode::getName() const {
    return name;
}

const Token& ASTNode::getToken() const {
    return token;
}

bool ASTNode::getIsTerminal() const {
    return isTerminal;
}

const std::vector<std::unique_ptr<ASTNode>>& ASTNode::getChildren() const {
    return children;
}

void ASTNode::addChild(std::unique_ptr<ASTNode> child) {
    if (child) {
        children.push_back(std::move(child));
    }
}

void ASTNode::addChild(ASTNode* child) {
    if (child) {
        children.push_back(std::unique_ptr<ASTNode>(child));
    }
}

std::string ASTNode::toGraphViz(int& nodeCounter) const {
    std::stringstream ss;
    int currentId = nodeCounter++;
    
    std::string nodeLabel = name;
    
    // Escapar caracteres especiales
    size_t pos = 0;
    while ((pos = nodeLabel.find('"', pos)) != std::string::npos) {
        nodeLabel.replace(pos, 1, "\\\"");
        pos += 2;
    }
    
    if (isTerminal) {
        std::string lexeme = token.getLexeme();
        if (lexeme.length() > 30) {
            lexeme = lexeme.substr(0, 27) + "...";
        }
        pos = 0;
        while ((pos = lexeme.find('"', pos)) != std::string::npos) {
            lexeme.replace(pos, 1, "\\\"");
            pos += 2;
        }
        ss << "  node" << currentId << " [shape=box, style=filled, fillcolor=\"#90EE90\", label=\"" 
           << nodeLabel << "\\n" << lexeme << "\"];\n";
    } else {
        ss << "  node" << currentId << " [shape=ellipse, style=filled, fillcolor=\"#87CEEB\", label=\""
           << nodeLabel << "\"];\n";
    }
    
    for (const auto& child : children) {
        int childId = nodeCounter;
        ss << child->toGraphViz(nodeCounter);
        ss << "  node" << currentId << " -> node" << childId << ";\n";
    }
    
    return ss.str();
}

void ASTNode::printTree(int depth) const {
    for (int i = 0; i < depth; i++) {
        std::cout << "  ";
    }
    
    if (isTerminal) {
        std::cout << "├─[" << name << ": \"" << token.getLexeme() << "\"]" << std::endl;
    } else {
        std::cout << "├─<" << name << ">" << std::endl;
    }
    
    for (const auto& child : children) {
        child->printTree(depth + 1);
    }
}

int ASTNode::getNodeCount() const {
    int count = 1;
    for (const auto& child : children) {
        count += child->getNodeCount();
    }
    return count;
}

} // namespace TaskScript