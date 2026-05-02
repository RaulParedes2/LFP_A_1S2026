#include "SyntaxAnalyzer.h"
#include <iostream>
#include <sstream>

namespace TaskScript
{

    SyntaxAnalyzer::SyntaxAnalyzer(LexicalAnalyzer &lexer, ErrorManager &errors)
        : lexer(lexer),
          errors(errors),
          currentToken(TokenType::FIN_DE_ARCHIVO, "", 0, 0),
          hasSyntacticError(false) {}

    Token SyntaxAnalyzer::peek() const
    {
        return currentToken;
    }

    bool SyntaxAnalyzer::check(TokenType type) const
    {
        return currentToken.getType() == type;
    }

    void SyntaxAnalyzer::advance()
    {
        if (currentToken.getType() != TokenType::FIN_DE_ARCHIVO)
        {
            currentToken = lexer.getNextToken();
        }
    }

    bool SyntaxAnalyzer::match(TokenType type)
    {
        if (check(type))
        {
            advance();
            return true;
        }
        return false;
    }

    void SyntaxAnalyzer::consume(TokenType expected, const std::string &errorMessage)
    {
        if (check(expected))
        {
            advance();
        }
        else
        {
            hasSyntacticError = true;
            errors.addError(currentToken.getLexeme(), ErrorType::SINTACTICO,
                            errorMessage + " Se encontro: " + currentToken.typeToString(),
                            currentToken.getLine(), currentToken.getColumn(),
                            ErrorSeverity::ERROR);
        }
    }

    void SyntaxAnalyzer::synchronize(const std::vector<TokenType> &syncTokens)
    {
        while (currentToken.getType() != TokenType::FIN_DE_ARCHIVO)
        {
            for (TokenType syncToken : syncTokens)
            {
                if (check(syncToken))
                {
                    return;
                }
            }
            advance();
        }
    }

    void SyntaxAnalyzer::synchronizeToNextValid()
    {
        std::vector<TokenType> syncPoints = {
            TokenType::TABLERO, TokenType::COLUMNA, TokenType::TAREA,
            TokenType::LLAVE_ABRE, TokenType::LLAVE_CIERRA, TokenType::FIN_DE_ARCHIVO};
        synchronize(syncPoints);
    }

    std::unique_ptr<ASTNode> SyntaxAnalyzer::parsePrioridad()
    {
        auto node = std::make_unique<ASTNode>("prioridad");

        if (check(TokenType::ALTA))
        {
            node->addChild(std::make_unique<ASTNode>(currentToken));
            advance();
        }
        else if (check(TokenType::MEDIA))
        {
            node->addChild(std::make_unique<ASTNode>(currentToken));
            advance();
        }
        else if (check(TokenType::BAJA))
        {
            node->addChild(std::make_unique<ASTNode>(currentToken));
            advance();
        }
        else
        {
            hasSyntacticError = true;
            errors.addError(currentToken.getLexeme(), ErrorType::SINTACTICO,
                            "Se esperaba ALTA, MEDIA o BAJA. Se encontro: " + currentToken.typeToString(),
                            currentToken.getLine(), currentToken.getColumn(),
                            ErrorSeverity::ERROR);
            auto errorNode = std::make_unique<ASTNode>("ERROR");
            node->addChild(std::move(errorNode));
        }

        return node;
    }

    std::unique_ptr<ASTNode> SyntaxAnalyzer::parseAtributo()
    {
        auto node = std::make_unique<ASTNode>("atributo");

        if (check(TokenType::PRIORIDAD))
        {
            node->addChild(std::make_unique<ASTNode>(currentToken));
            advance();
            consume(TokenType::DOS_PUNTOS, "Se esperaba ':' despues de 'prioridad'");
            node->addChild(parsePrioridad());
        }
        else if (check(TokenType::RESPONSABLE))
        {
            node->addChild(std::make_unique<ASTNode>(currentToken));
            advance();
            consume(TokenType::DOS_PUNTOS, "Se esperaba ':' despues de 'responsable'");

            if (check(TokenType::CADENA))
            {
                node->addChild(std::make_unique<ASTNode>(currentToken));
                advance();
            }
            else
            {
                hasSyntacticError = true;
                errors.addError(currentToken.getLexeme(), ErrorType::SINTACTICO,
                                "Se esperaba una cadena para el responsable",
                                currentToken.getLine(), currentToken.getColumn(),
                                ErrorSeverity::ERROR);
            }
        }
        else if (check(TokenType::FECHA_LIMITE))
        {
            node->addChild(std::make_unique<ASTNode>(currentToken));
            advance();
            consume(TokenType::DOS_PUNTOS, "Se esperaba ':' despues de 'fecha_limite'");

            if (check(TokenType::FECHA))
            {
                node->addChild(std::make_unique<ASTNode>(currentToken));
                advance();
            }
            else
            {
                hasSyntacticError = true;
                errors.addError(currentToken.getLexeme(), ErrorType::SINTACTICO,
                                "Se esperaba una fecha en formato AAAA-MM-DD",
                                currentToken.getLine(), currentToken.getColumn(),
                                ErrorSeverity::ERROR);
            }
        }
        else
        {
            hasSyntacticError = true;
            errors.addError(currentToken.getLexeme(), ErrorType::SINTACTICO,
                            "Se esperaba 'prioridad', 'responsable' o 'fecha_limite'",
                            currentToken.getLine(), currentToken.getColumn(),
                            ErrorSeverity::ERROR);
            synchronizeToNextValid();
        }

        return node;
    }

    std::unique_ptr<ASTNode> SyntaxAnalyzer::parseAtributos()
    {
        auto node = std::make_unique<ASTNode>("atributos");

        node->addChild(parseAtributo());

        while (check(TokenType::COMA))
        {
            node->addChild(std::make_unique<ASTNode>(currentToken));
            advance();
            node->addChild(parseAtributo());
        }

        return node;
    }

    std::unique_ptr<ASTNode> SyntaxAnalyzer::parseTarea()
    {
        auto node = std::make_unique<ASTNode>("tarea");

        consume(TokenType::TAREA, "Se esperaba la palabra reservada 'tarea'");
        node->addChild(std::make_unique<ASTNode>(currentToken));

        consume(TokenType::DOS_PUNTOS, "Se esperaba ':' despues de 'tarea'");

        if (check(TokenType::CADENA))
        {
            node->addChild(std::make_unique<ASTNode>(currentToken));
            advance();
        }
        else
        {
            hasSyntacticError = true;
            errors.addError(currentToken.getLexeme(), ErrorType::SINTACTICO,
                            "Se esperaba el nombre de la tarea entre comillas",
                            currentToken.getLine(), currentToken.getColumn(),
                            ErrorSeverity::ERROR);
        }

        consume(TokenType::CORCHETE_ABRE, "Se esperaba '[' para iniciar los atributos");
        node->addChild(parseAtributos());
        consume(TokenType::CORCHETE_CIERRA, "Se esperaba ']' para cerrar los atributos");

        return node;
    }

    std::unique_ptr<ASTNode> SyntaxAnalyzer::parseTareas()
    {
        auto node = std::make_unique<ASTNode>("tareas");

        node->addChild(parseTarea());

        while (check(TokenType::COMA))
        {
            node->addChild(std::make_unique<ASTNode>(currentToken));
            advance();
            node->addChild(parseTarea());
        }

        return node;
    }

    std::unique_ptr<ASTNode> SyntaxAnalyzer::parseColumna()
    {
        auto node = std::make_unique<ASTNode>("columna");

        consume(TokenType::COLUMNA, "Se esperaba la palabra reservada 'COLUMNA'");
        node->addChild(std::make_unique<ASTNode>(currentToken));

        if (check(TokenType::CADENA))
        {
            node->addChild(std::make_unique<ASTNode>(currentToken));
            advance();
        }
        else
        {
            hasSyntacticError = true;
            errors.addError(currentToken.getLexeme(), ErrorType::SINTACTICO,
                            "Se esperaba el nombre de la columna entre comillas",
                            currentToken.getLine(), currentToken.getColumn(),
                            ErrorSeverity::ERROR);
        }

        consume(TokenType::LLAVE_ABRE, "Se esperaba '{' para abrir el bloque de la columna");

        if (check(TokenType::TAREA))
        {
            node->addChild(parseTareas());
        }
        else
        {
            auto emptyTareas = std::make_unique<ASTNode>("tareas_vacio");
            node->addChild(std::move(emptyTareas));
        }

        consume(TokenType::LLAVE_CIERRA, "Se esperaba '}' para cerrar el bloque de la columna");
        consume(TokenType::PUNTO_Y_COMA, "Se esperaba ';' al final de la columna");

        return node;
    }

    std::unique_ptr<ASTNode> SyntaxAnalyzer::parseColumnas()
    {
        auto node = std::make_unique<ASTNode>("columnas");

        node->addChild(parseColumna());

        while (check(TokenType::COLUMNA))
        {
            node->addChild(parseColumna());
        }

        return node;
    }

    std::unique_ptr<ASTNode> SyntaxAnalyzer::parsePrograma()
    {
        auto node = std::make_unique<ASTNode>("programa");

        consume(TokenType::TABLERO, "Se esperaba 'TABLERO' al inicio del archivo");
        node->addChild(std::make_unique<ASTNode>(currentToken));

        if (check(TokenType::CADENA))
        {
            node->addChild(std::make_unique<ASTNode>(currentToken));
            advance();
        }
        else
        {
            hasSyntacticError = true;
            errors.addError(currentToken.getLexeme(), ErrorType::SINTACTICO,
                            "Se esperaba el nombre del tablero entre comillas",
                            currentToken.getLine(), currentToken.getColumn(),
                            ErrorSeverity::ERROR);
        }

        consume(TokenType::LLAVE_ABRE, "Se esperaba '{' para abrir el bloque del tablero");
        node->addChild(parseColumnas());
        consume(TokenType::LLAVE_CIERRA, "Se esperaba '}' para cerrar el bloque del tablero");
        consume(TokenType::PUNTO_Y_COMA, "Se esperaba ';' al final del tablero");

        return node;
    }

    std::unique_ptr<ASTNode> SyntaxAnalyzer::parse()
    {
        reset();

        // Obtener el primer token
        advance();

        // Iniciar el parsing
        root = parsePrograma();

        // Verificar que se hayan consumido todos los tokens
        if (currentToken.getType() != TokenType::FIN_DE_ARCHIVO)
        {
            hasSyntacticError = true;
            errors.addError(currentToken.getLexeme(), ErrorType::SINTACTICO,
                            "Tokens adicionales despues del final esperado del programa",
                            currentToken.getLine(), currentToken.getColumn(),
                            ErrorSeverity::ERROR);
        }

        if (hasSyntacticError)
        {
            return nullptr;
        }

        return std::move(root);
    }

    bool SyntaxAnalyzer::isSuccessful() const
    {
        return !hasSyntacticError && root != nullptr;
    }

    std::string SyntaxAnalyzer::generateDotCode() const
    {
        std::stringstream ss;

        if (!root)
        {
            ss << "digraph ArbolDerivacion {\n";
            ss << "  label=\"No se pudo generar el arbol por errores sintacticos\";\n";
            ss << "  labelloc=t;\n";
            ss << "  fontsize=14;\n";
            ss << "}\n";
            return ss.str();
        }

        // Resetear el contador antes de generar el DOT
        ASTNode::resetDotCounter();

        ss << "digraph ArbolDerivacion {\n";
        ss << "  rankdir=TB;\n";
        ss << "  node [fontname=\"Arial\", fontsize=10];\n";
        ss << "  edge [arrowhead=normal];\n\n";

        ss << root->toGraphViz();

        ss << "}\n";
        return ss.str();
    }

    void SyntaxAnalyzer::reset()
    {
        hasSyntacticError = false;
        root = nullptr;
    }

} // namespace TaskScript