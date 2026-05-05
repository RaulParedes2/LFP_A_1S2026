#ifndef SYNTAX_ANALYZER_H
#define SYNTAX_ANALYZER_H

#include "../Lexer/LexicalAnalyzer.h"
#include "../Error/ErrorManager.h"
#include "ASTNode.h"
#include <memory>
#include <vector>
#include <string>

using namespace std;

namespace TaskScript
{

    class SyntaxAnalyzer
    {
        private:
        LexicalAnalyzer& lexer;
        ErrorManager& errors;
        Token currentToken;
        unique_ptr<ASTNode> root;
        bool hasSyntacticError;

        void advance();
        bool check(TokenType type) const;
        bool match(TokenType type);
        Token consume(TokenType expected, const std::string &errorMessage);
        Token peek() const;

        void synchronize(const std::vector<TokenType> &syncTokens);
        void synchronizeToNextValid();

        unique_ptr<ASTNode> parsePrograma();
        unique_ptr<ASTNode> parseColumnas();
        unique_ptr<ASTNode> parseColumna();
        unique_ptr<ASTNode> parseTareas();
        unique_ptr<ASTNode> parseTarea();
        unique_ptr<ASTNode> parseAtributos();
        unique_ptr<ASTNode> parseAtributo();
        unique_ptr<ASTNode> parsePrioridad();

    public:
        SyntaxAnalyzer(LexicalAnalyzer &lexer, ErrorManager &errors);
        
        // El parse retorna el arbol pero NO mueve root
        unique_ptr<ASTNode> parse();
        
        // Metodo para obtener el arbol sin moverlo
        ASTNode* getAST() const { return root.get(); }
        
        bool isSuccessful() const;
        string generateDotCode() const;
        void reset();
    };
}
#endif