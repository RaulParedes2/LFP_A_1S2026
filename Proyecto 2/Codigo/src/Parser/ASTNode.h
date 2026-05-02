#ifndef AST_NODE_H
#define AST_NODE_H

#include "../Lexer/Token.h"
#include <vector>
#include <memory>
#include <string>

namespace TaskScript
{

    class ASTNode
    {
    private:
        std::string name;
        Token token;
        bool isTerminal;
        std::vector<std::unique_ptr<ASTNode>> children;

        // Variable estatica para el contador de nodos en toGraphViz
        static int dotNodeCounter;

    public:
        explicit ASTNode(const std::string &productionName);
        explicit ASTNode(const Token &token);
        ASTNode(const std::string &name, const Token &token);

        // Movimiento
        ASTNode(ASTNode &&other) noexcept;
        ASTNode &operator=(ASTNode &&other) noexcept;

        // Eliminar copia
        ASTNode(const ASTNode &) = delete;
        ASTNode &operator=(const ASTNode &) = delete;

        // Getters
        const std::string &getName() const;
        const Token &getToken() const;
        bool getIsTerminal() const;
        const std::vector<std::unique_ptr<ASTNode>> &getChildren() const;

        // Metodos
        void addChild(std::unique_ptr<ASTNode> child);
        void addChild(ASTNode *child);

        std::string toGraphViz() const;
        void printTree(int depth = 0) const;
        int getNodeCount() const;

        // Metodo estatico para resetear el contador de DOT
        static void resetDotCounter();
    };

} // namespace TaskScript

#endif // AST_NODE_H