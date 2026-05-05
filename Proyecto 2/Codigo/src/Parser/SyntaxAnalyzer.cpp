#include "SyntaxAnalyzer.h"
#include <iostream>
#include <sstream>

namespace TaskScript
{

    SyntaxAnalyzer::SyntaxAnalyzer(LexicalAnalyzer &lexer, ErrorManager &errors)
        : lexer(lexer),
          errors(errors),
          currentToken(),
          hasSyntacticError(false) {}

    Token SyntaxAnalyzer::peek() const
    {
        return currentToken;
    }

    bool SyntaxAnalyzer::check(TokenType type) const
    {
        return currentToken.getType() == type;
    }

    /*void SyntaxAnalyzer::advance()
    {
        if (currentToken.getType() != TokenType::FIN_DE_ARCHIVO)
        {
            currentToken = lexer.getNextToken();
        }
    }*/

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

    Token SyntaxAnalyzer::consume(TokenType expected, const std::string &errorMessage)
    {
        if (check(expected))
        {
            Token consume = currentToken; //
            advance();
            return consume;
        }
        else
        {
            hasSyntacticError = true;
            errors.addError(currentToken.getLexeme(), ErrorType::SINTACTICO_ERROR,
                            errorMessage + " Se encontro: " + currentToken.typeToString(),
                            currentToken.getLine(), currentToken.getColumn(),
                            ErrorSeverity::ERROR_NORMAL);

            return currentToken; // retorno dummy
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

    /*std::unique_ptr<ASTNode> SyntaxAnalyzer::parsePrioridad() {
        auto node = std::make_unique<ASTNode>("prioridad");

        if (check(TokenType::ALTA)) {
            node->addChild(std::make_unique<ASTNode>(currentToken));
            advance();
        } else if (check(TokenType::MEDIA)) {
            node->addChild(std::make_unique<ASTNode>(currentToken));
            advance();
        } else if (check(TokenType::BAJA)) {
            node->addChild(std::make_unique<ASTNode>(currentToken));
            advance();
        } else {
            hasSyntacticError = true;
            errors.addError(currentToken.getLexeme(), ErrorType::SINTACTICO_ERROR,
                           "Se esperaba ALTA, MEDIA o BAJA. Se encontro: " + currentToken.typeToString(),
                           currentToken.getLine(), currentToken.getColumn(),
                           ErrorSeverity::ERROR_NORMAL);
            auto errorNode = std::make_unique<ASTNode>("ERROR");
            node->addChild(std::move(errorNode));
        }

        return node;
    }*/

    /*std::unique_ptr<ASTNode> SyntaxAnalyzer::parsePrioridad()
    {
        auto node = std::make_unique<ASTNode>("prioridad");

        // Si es DESCONOCIDO, recuperarse sin error adicional
        if (currentToken.getType() == TokenType::DESCONOCIDO)
        {
            advance();
            return node;
        }

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
            errors.addError(currentToken.getLexeme(), ErrorType::SINTACTICO_ERROR,
                            "Se esperaba ALTA, MEDIA o BAJA. Se encontro: " + currentToken.typeToString(),
                            currentToken.getLine(), currentToken.getColumn(),
                            ErrorSeverity::ERROR_NORMAL);
            advance();
        }

        return node;
    }*/

    std::unique_ptr<ASTNode> SyntaxAnalyzer::parsePrioridad()
    {
        auto node = std::make_unique<ASTNode>("prioridad");

        // Si es DESCONOCIDO, recuperarse y salir sin error adicional
        if (currentToken.getType() == TokenType::DESCONOCIDO)
        {
            advance(); // Consumir el token erróneo
            return node;
        }

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
            errors.addError(currentToken.getLexeme(), ErrorType::SINTACTICO_ERROR,
                            "Se esperaba ALTA, MEDIA o BAJA. Se encontro: " + currentToken.typeToString(),
                            currentToken.getLine(), currentToken.getColumn(),
                            ErrorSeverity::ERROR_NORMAL);
            advance();
        }

        return node;
    }

    /*std::unique_ptr<ASTNode> SyntaxAnalyzer::parseAtributo()
    {
        auto node = std::make_unique<ASTNode>("atributo");

        // Si es DESCONOCIDO, reportar SOLO UN error y continuar
        if (currentToken.getType() == TokenType::DESCONOCIDO)
        {
            // Consumir hasta encontrar ',' , ']' o '}'
            while (!check(TokenType::COMA) && !check(TokenType::CORCHETE_CIERRA) &&
                   !check(TokenType::LLAVE_CIERRA) && !check(TokenType::FIN_DE_ARCHIVO))
            {
                advance();
            }
            advance(); // Consumir la coma o corchete
            return node;
        }*/

    /*std::unique_ptr<ASTNode> SyntaxAnalyzer::parseAtributo()
    {
        auto node = std::make_unique<ASTNode>("atributo");

        // Si es DESCONOCIDO (error léxico), consumir hasta el final del atributo
        if (currentToken.getType() == TokenType::DESCONOCIDO)
        {
            // Consumir el token erróneo
            advance();

            // Consumir hasta encontrar ',' , ']' o '}'
            while (!check(TokenType::COMA) && !check(TokenType::CORCHETE_CIERRA) &&
                   !check(TokenType::LLAVE_CIERRA) && !check(TokenType::FIN_DE_ARCHIVO))
            {
                advance();
            }

            // Si hay coma, consumirla también
            if (check(TokenType::COMA))
            {
                advance();
            }

            return node;
        }

        /*  if (check(TokenType::PRIORIDAD))
          {
              node->addChild(std::make_unique<ASTNode>(currentToken));
              advance();

              // Verificar si hay ':' después de prioridad
              if (!check(TokenType::DOS_PUNTOS))
              {
                  hasSyntacticError = true;
                  // Este error debe reportarse con el lexema que sigue a prioridad
                  std::string foundToken = currentToken.typeToString();
                  if (currentToken.getType() == TokenType::ALTA)
                      foundToken = "ALTA";
                  else if (currentToken.getType() == TokenType::MEDIA)
                      foundToken = "MEDIA";
                  else if (currentToken.getType() == TokenType::BAJA)
                      foundToken = "BAJA";

                  errors.addError(currentToken.getLexeme(), ErrorType::SINTACTICO_ERROR,
                                  "Se esperaba ':' después de 'prioridad', se encontró: " + foundToken,
                                  currentToken.getLine(), currentToken.getColumn(),
                                  ErrorSeverity::ERROR_NORMAL);

                  // Recuperación: consumir hasta ',' , ']' o '}'
                  while (!check(TokenType::COMA) && !check(TokenType::CORCHETE_CIERRA) &&
                         !check(TokenType::LLAVE_CIERRA) && !check(TokenType::FIN_DE_ARCHIVO))
                  {
                      advance();
                  }
                  return node;
              }

              consume(TokenType::DOS_PUNTOS, "Se esperaba ':' despues de 'prioridad'");
              node->addChild(parsePrioridad());
          }*/
    /*
            if (check(TokenType::PRIORIDAD))
            {
                node->addChild(std::make_unique<ASTNode>(currentToken));
                advance();

                if (!check(TokenType::DOS_PUNTOS))
                {
                    hasSyntacticError = true;

                    std::string foundToken = currentToken.typeToString();
                    if (currentToken.getType() == TokenType::ALTA)
                        foundToken = "ALTA";
                    else if (currentToken.getType() == TokenType::MEDIA)
                        foundToken = "MEDIA";
                    else if (currentToken.getType() == TokenType::BAJA)
                        foundToken = "BAJA";

                    errors.addError(currentToken.getLexeme(), ErrorType::SINTACTICO_ERROR,
                                    "Se esperaba ':' después de 'prioridad', se encontró: " + foundToken,
                                    currentToken.getLine(), currentToken.getColumn(),
                                    ErrorSeverity::ERROR_NORMAL);

                    // Consumir hasta el final
                    while (!check(TokenType::CORCHETE_CIERRA) && !check(TokenType::LLAVE_CIERRA) &&
                           !check(TokenType::FIN_DE_ARCHIVO))
                    {
                        advance();
                    }

                    return node;
                }

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
                    errors.addError(currentToken.getLexeme(), ErrorType::SINTACTICO_ERROR,
                                    "Se esperaba una cadena para el responsable",
                                    currentToken.getLine(), currentToken.getColumn(),
                                    ErrorSeverity::ERROR_NORMAL);
                    advance();
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
                    errors.addError(currentToken.getLexeme(), ErrorType::SINTACTICO_ERROR,
                                    "Se esperaba una fecha en formato AAAA-MM-DD",
                                    currentToken.getLine(), currentToken.getColumn(),
                                    ErrorSeverity::ERROR_NORMAL);
                    advance();
                }
            }
            else
            {
                hasSyntacticError = true;
                errors.addError(currentToken.getLexeme(), ErrorType::SINTACTICO_ERROR,
                                "Se esperaba 'prioridad', 'responsable' o 'fecha_limite'",
                                currentToken.getLine(), currentToken.getColumn(),
                                ErrorSeverity::ERROR_NORMAL);
                advance();
            }

            return node;
        }
        */

    std::unique_ptr<ASTNode> SyntaxAnalyzer::parseAtributo()
{
    auto node = std::make_unique<ASTNode>("atributo");

    // Si es DESCONOCIDO (error léxico), recuperarse y salir
    if (currentToken.getType() == TokenType::DESCONOCIDO)
    {
        advance();  // Consumir el token erróneo
        
        // Consumir hasta encontrar ',' , ']' o '}'
        while (!check(TokenType::COMA) && !check(TokenType::CORCHETE_CIERRA) && 
               !check(TokenType::LLAVE_CIERRA) && !check(TokenType::FIN_DE_ARCHIVO))
        {
            advance();
        }
        
        // Si hay una coma, consumirla también
        if (check(TokenType::COMA))
        {
            advance();
        }
        
        return node;
    }

    // Resto del código normal...
    if (check(TokenType::PRIORIDAD))
    {
        node->addChild(std::make_unique<ASTNode>(currentToken));
        advance();
        
        if (!check(TokenType::DOS_PUNTOS))
        {
            hasSyntacticError = true;
            
            std::string foundToken = currentToken.typeToString();
            if (currentToken.getType() == TokenType::ALTA) foundToken = "ALTA";
            else if (currentToken.getType() == TokenType::MEDIA) foundToken = "MEDIA";
            else if (currentToken.getType() == TokenType::BAJA) foundToken = "BAJA";
            
            errors.addError(currentToken.getLexeme(), ErrorType::SINTACTICO_ERROR,
                            "Se esperaba ':' después de 'prioridad', se encontró: " + foundToken,
                            currentToken.getLine(), currentToken.getColumn(),
                            ErrorSeverity::ERROR_NORMAL);
            
            // Consumir hasta el final
            while (!check(TokenType::CORCHETE_CIERRA) && !check(TokenType::LLAVE_CIERRA) && 
                   !check(TokenType::FIN_DE_ARCHIVO))
            {
                advance();
            }
            
            return node;
        }
        
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
            errors.addError(currentToken.getLexeme(), ErrorType::SINTACTICO_ERROR,
                            "Se esperaba una cadena para el responsable",
                            currentToken.getLine(), currentToken.getColumn(),
                            ErrorSeverity::ERROR_NORMAL);
            advance();
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
            errors.addError(currentToken.getLexeme(), ErrorType::SINTACTICO_ERROR,
                            "Se esperaba una fecha en formato AAAA-MM-DD",
                            currentToken.getLine(), currentToken.getColumn(),
                            ErrorSeverity::ERROR_NORMAL);
            advance();
        }
    }
    else
    {
        hasSyntacticError = true;
        errors.addError(currentToken.getLexeme(), ErrorType::SINTACTICO_ERROR,
                        "Se esperaba 'prioridad', 'responsable' o 'fecha_limite'",
                        currentToken.getLine(), currentToken.getColumn(),
                        ErrorSeverity::ERROR_NORMAL);
        advance();
    }

    return node;
}

std::unique_ptr<ASTNode> SyntaxAnalyzer::parseAtributos()
{
    auto node = std::make_unique<ASTNode>("atributos");

    auto primero = parseAtributo();
    if (!primero || hasSyntacticError)
    {
        return node;
    }
    node->addChild(std::move(primero));

    while (check(TokenType::COMA))
    {
        advance();

        if (hasSyntacticError)
        {
            break;
        }

        auto siguiente = parseAtributo();
        if (!siguiente)
        {
            break;
        }
        node->addChild(std::move(siguiente));
    }

    return node;
}
    /*

    std::unique_ptr<ASTNode> SyntaxAnalyzer::parseAtributos()
    {
        auto node = std::make_unique<ASTNode>("atributos");

        // Intentar parsear el primer atributo
        auto primero = parseAtributo();
        if (!primero || hasSyntacticError)
        {
            return node;
        }
        node->addChild(std::move(primero));

        while (check(TokenType::COMA))
        {
            advance();

            // Si hay error previo, salir
            if (hasSyntacticError)
            {
                break;
            }

            auto siguiente = parseAtributo();
            if (!siguiente)
            {
                break;
            }
            node->addChild(std::move(siguiente));
        }

        return node;
    }
    */
    std::unique_ptr<ASTNode> SyntaxAnalyzer::parseTarea()
    {
        auto node = std::make_unique<ASTNode>("tarea");

        Token t = consume(TokenType::TAREA, "Se esperaba 'tarea'");
        node->addChild(std::make_unique<ASTNode>(t));

        consume(TokenType::DOS_PUNTOS, "Se esperaba ':' despues de 'tarea'");

        if (check(TokenType::CADENA))
        {
            node->addChild(std::make_unique<ASTNode>(currentToken));
            advance();
        }
        else
        {
            hasSyntacticError = true;
            errors.addError(currentToken.getLexeme(), ErrorType::SINTACTICO_ERROR,
                            "Se esperaba el nombre de la tarea entre comillas",
                            currentToken.getLine(), currentToken.getColumn(),
                            ErrorSeverity::ERROR_NORMAL);
        }

        if (check(TokenType::CORCHETE_ABRE))
        {
            node->addChild(std::make_unique<ASTNode>(currentToken));
            advance();
            node->addChild(parseAtributos());
            consume(TokenType::CORCHETE_CIERRA, "Se esperaba ']' para cerrar los atributos");
        }

        return node;
    }

    /*std::unique_ptr<ASTNode> SyntaxAnalyzer::parseTareas() {
        auto node = std::make_unique<ASTNode>("tareas");

        node->addChild(parseTarea());

        while (check(TokenType::COMA)) {
            node->addChild(std::make_unique<ASTNode>(currentToken));
            advance();
            node->addChild(parseTarea());
        }

        return node;
    }*/

    std::unique_ptr<ASTNode> SyntaxAnalyzer::parseTareas()
    {
        auto node = std::make_unique<ASTNode>("tareas");

        // Al menos una tarea
        node->addChild(parseTarea());

        // Mientras haya coma, seguimos
        while (check(TokenType::COMA))
        {
            advance();
            // Permitir coma trailing: si no hay más tareas, salimos
            if (!check(TokenType::TAREA))
            {
                break; // Coma trailing permitida
            }
            node->addChild(parseTarea());
        }

        return node;
    }

    std::unique_ptr<ASTNode> SyntaxAnalyzer::parseColumna()
    {
        auto node = std::make_unique<ASTNode>("columna");

        Token t = consume(TokenType::COLUMNA, "Se esperaba 'COLUMNA'");
        node->addChild(std::make_unique<ASTNode>(t));

        if (check(TokenType::CADENA))
        {
            node->addChild(std::make_unique<ASTNode>(currentToken));
            advance();
        }
        else
        {
            hasSyntacticError = true;
            errors.addError(currentToken.getLexeme(), ErrorType::SINTACTICO_ERROR,
                            "Se esperaba el nombre de la columna entre comillas",
                            currentToken.getLine(), currentToken.getColumn(),
                            ErrorSeverity::ERROR_NORMAL);
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

        Token t = consume(TokenType::TABLERO, "Se esperaba 'TABLERO'");
        node->addChild(std::make_unique<ASTNode>(t));

        if (check(TokenType::CADENA))
        {
            node->addChild(std::make_unique<ASTNode>(currentToken));
            advance();
        }
        else
        {
            hasSyntacticError = true;
            errors.addError(currentToken.getLexeme(), ErrorType::SINTACTICO_ERROR,
                            "Se esperaba el nombre del tablero entre comillas",
                            currentToken.getLine(), currentToken.getColumn(),
                            ErrorSeverity::ERROR_NORMAL);
        }

        consume(TokenType::LLAVE_ABRE, "Se esperaba '{' para abrir el bloque del tablero");
        node->addChild(parseColumnas());
        consume(TokenType::LLAVE_CIERRA, "Se esperaba '}' para cerrar el bloque del tablero");
        consume(TokenType::PUNTO_Y_COMA, "Se esperaba ';' al final del tablero");

        return node;
    }

    /* std::unique_ptr<ASTNode> SyntaxAnalyzer::parse()
    {
        reset();

        currentToken = lexer.getNextToken();

        //  SOLUCIÓN CLAVE: detener si hay errores léxicos
        if (errors.hasLexicalErrors())
        {
            hasSyntacticError = true;
            return nullptr;
        }

        if (currentToken.getType() == TokenType::FIN_DE_ARCHIVO)
        {
            hasSyntacticError = true;
            errors.addError("", ErrorType::SINTACTICO_ERROR,
                            "El archivo esta vacio o no se pudieron leer tokens",
                            0, 0, ErrorSeverity::CRITICO_ERROR);
            return nullptr;
        }

        root = parsePrograma();

        if (currentToken.getType() != TokenType::FIN_DE_ARCHIVO)
        {
            hasSyntacticError = true;
            errors.addError(currentToken.getLexeme(), ErrorType::SINTACTICO_ERROR,
                            "Tokens adicionales despues del final esperado del programa",
                            currentToken.getLine(), currentToken.getColumn(),
                            ErrorSeverity::ERROR_NORMAL);
        }

        if (hasSyntacticError)
        {
            return nullptr;
        }

        return std::unique_ptr<ASTNode>(root.get());
    }*/

    void SyntaxAnalyzer::reset()
    {
        hasSyntacticError = false;
        root = nullptr;
    }

    std::string SyntaxAnalyzer::generateDotCode() const
    {
        std::stringstream ss;

        if (!root)
        {
            ss << "digraph ArbolDerivacion {\n";
            ss << "  label=\"No se pudo generar el arbol\";\n";
            ss << "  labelloc=t;\n";
            ss << "  fontsize=14;\n";
            ss << "}\n";
            return ss.str();
        }

        ASTNode::resetDotCounter();

        ss << "digraph ArbolDerivacion {\n";
        ss << "  rankdir=TB;\n";
        ss << "  node [fontname=\"Arial\", fontsize=10];\n";
        ss << "  edge [arrowhead=normal];\n\n";

        int nodeCounter = 0;
        ss << root->toGraphViz(nodeCounter);

        ss << "}\n";
        return ss.str();
    }

    std::unique_ptr<ASTNode> SyntaxAnalyzer::parse()
    {
        reset();

        // NO detenerse en errores léxicos, acumularlos todos
        // errors.hasLexicalErrors() NO debe detener el análisis

        currentToken = lexer.getNextToken();

        if (currentToken.getType() == TokenType::FIN_DE_ARCHIVO)
        {
            hasSyntacticError = true;
            errors.addError("", ErrorType::SINTACTICO_ERROR,
                            "El archivo esta vacio o no se pudieron leer tokens",
                            0, 0, ErrorSeverity::CRITICO_ERROR);
            return nullptr;
        }

        root = parsePrograma();

        if (currentToken.getType() != TokenType::FIN_DE_ARCHIVO)
        {
            hasSyntacticError = true;
            errors.addError(currentToken.getLexeme(), ErrorType::SINTACTICO_ERROR,
                            "Tokens adicionales despues del final esperado del programa",
                            currentToken.getLine(), currentToken.getColumn(),
                            ErrorSeverity::ERROR_NORMAL);
        }

        if (hasSyntacticError)
        {
            return nullptr;
        }

        return std::unique_ptr<ASTNode>(root.get());
    }

} // namespace TaskScript