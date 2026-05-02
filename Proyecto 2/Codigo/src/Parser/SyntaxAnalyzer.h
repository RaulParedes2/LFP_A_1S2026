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

    // Analizador sintactico implementado con parser descendente recursivo
    // Cada no-terminal de la gramatica tiene su propio metodo

    class SyntaxAnalyzer
    {
        private:
        LexicalAnalyzer& lexer;   // Referencia al analizador lexico
        ErrorManager& errors;     // Referencia al manejador de errores
        Token currentToken;       // Token actual siendo analizado
        unique_ptr<ASTNode> root; // Raiz del arbol de sintaxis abstracta
        bool hasSyntacticError;      // Bandera para indicar si se encontro un error sintactico

        // Metodos auxiliares para manejo de tokens
        void advance();                                                    // Avanza al siguiente token
        bool check(TokenType type) const;                                  // Verifica si el token actual es de cierto tipo
        bool match(TokenType type);                                        // Verifica y consume si coincide
        void consume(TokenType expected, const std::string &errorMessage); // Consume o reporta error

        Token peek() const; // Retorna el token actual sin consumir

        // Metodos de sincronizacion para recuperacion de errores
        void synchronize(const std::vector<TokenType> &syncTokens);
        void synchronizeToNextValid();

        // Metodos para cada produccion de la gramatica (parser recursivo)
        unique_ptr<ASTNode> parsePrograma();   // <programa>
        unique_ptr<ASTNode> parseColumnas();   // <columnas>
        unique_ptr<ASTNode> parseColumna();   // <columna>
        unique_ptr<ASTNode> parseTareas();     // <tareas>
        unique_ptr<ASTNode> parseTarea();      // <tarea>
        unique_ptr<ASTNode> parseAtributos();  // <atributos>
        unique_ptr<ASTNode> parseAtributo();   // <atributo>
        unique_ptr<ASTNode> parsePrioridad(); // <prioridad>

    public:
        SyntaxAnalyzer(LexicalAnalyzer &lexer, ErrorManager &errors);
        // Inicia el proceso de parsing y retorna la raiz del arbol de derivacion
        unique_ptr<ASTNode> parse();

        // Verifica si el analisis fue exitoso (sin errores sintacticos)
        bool isSuccessful() const;

        // Genera el codigo DOT para Graphviz del arbol de derivacion
        string generateDotCode() const;

        // Reinicia el analizador
        void reset();
    };
}
#endif // SYNTAX_ANALYZER_H