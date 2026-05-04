#ifndef AST_NODE_H
#define AST_NODE_H

#include "../Lexer/Token.h"
#include <vector>
#include <memory>
#include <string>

namespace TaskScript {

class ASTNode {
private:
    std::string name;
    Token token;
    bool isTerminal;
    std::vector<std::unique_ptr<ASTNode>> children;
    
public:
    explicit ASTNode(const std::string& productionName);
    explicit ASTNode(const Token& token);
    ASTNode(const std::string& name, const Token& token);
    
    ~ASTNode() = default;
    
    ASTNode(ASTNode&& other) noexcept;
    ASTNode& operator=(ASTNode&& other) noexcept;
    
    ASTNode(const ASTNode&) = delete;
    ASTNode& operator=(const ASTNode&) = delete;
    
    const std::string& getName() const;
    const Token& getToken() const;
    bool getIsTerminal() const;
    const std::vector<std::unique_ptr<ASTNode>>& getChildren() const;
    
    void addChild(std::unique_ptr<ASTNode> child);
    void addChild(ASTNode* child);
    
    std::string toGraphViz(int& nodeCounter) const;
    void printTree(int depth = 0) const;
    int getNodeCount() const;
    
    static void resetDotCounter();
};

} // namespace TaskScript

#endif // AST_NODE_H