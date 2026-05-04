#include "SyntaxAnalyzer.h"
#include <iostream>
#include <sstream>

namespace TaskScript {

SyntaxAnalyzer::SyntaxAnalyzer(LexicalAnalyzer& lexer, ErrorManager& errors)
    : lexer(lexer), 
      errors(errors), 
      currentToken(),
      hasSyntacticError(false) {}

Token SyntaxAnalyzer::peek() const {
    return currentToken;
}

bool SyntaxAnalyzer::check(TokenType type) const {
    return currentToken.getType() == type;
}

void SyntaxAnalyzer::advance() {
    if (currentToken.getType() != TokenType::FIN_DE_ARCHIVO) {
        currentToken = lexer.getNextToken();
    }
}

bool SyntaxAnalyzer::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

void SyntaxAnalyzer::consume(TokenType expected, const std::string& errorMessage) {
    if (check(expected)) {
        advance();
    } else {
        hasSyntacticError = true;
        errors.addError(currentToken.getLexeme(), ErrorType::SINTACTICO_ERROR,
                       errorMessage + " Se encontro: " + currentToken.typeToString(),
                       currentToken.getLine(), currentToken.getColumn(),
                       ErrorSeverity::ERROR_NORMAL);
    }
}

void SyntaxAnalyzer::synchronize(const std::vector<TokenType>& syncTokens) {
    while (currentToken.getType() != TokenType::FIN_DE_ARCHIVO) {
        for (TokenType syncToken : syncTokens) {
            if (check(syncToken)) {
                return;
            }
        }
        advance();
    }
}

void SyntaxAnalyzer::synchronizeToNextValid() {
    std::vector<TokenType> syncPoints = {
        TokenType::TABLERO, TokenType::COLUMNA, TokenType::TAREA,
        TokenType::LLAVE_ABRE, TokenType::LLAVE_CIERRA, TokenType::FIN_DE_ARCHIVO
    };
    synchronize(syncPoints);
}

std::unique_ptr<ASTNode> SyntaxAnalyzer::parsePrioridad() {
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
}

std::unique_ptr<ASTNode> SyntaxAnalyzer::parseAtributo() {
    auto node = std::make_unique<ASTNode>("atributo");
    
    if (check(TokenType::PRIORIDAD)) {
        node->addChild(std::make_unique<ASTNode>(currentToken));
        advance();
        consume(TokenType::DOS_PUNTOS, "Se esperaba ':' despues de 'prioridad'");
        node->addChild(parsePrioridad());
    }
    else if (check(TokenType::RESPONSABLE)) {
        node->addChild(std::make_unique<ASTNode>(currentToken));
        advance();
        consume(TokenType::DOS_PUNTOS, "Se esperaba ':' despues de 'responsable'");
        
        if (check(TokenType::CADENA)) {
            node->addChild(std::make_unique<ASTNode>(currentToken));
            advance();
        } else {
            hasSyntacticError = true;
            errors.addError(currentToken.getLexeme(), ErrorType::SINTACTICO_ERROR,
                           "Se esperaba una cadena para el responsable",
                           currentToken.getLine(), currentToken.getColumn(),
                           ErrorSeverity::ERROR_NORMAL);
        }
    }
    else if (check(TokenType::FECHA_LIMITE)) {
        node->addChild(std::make_unique<ASTNode>(currentToken));
        advance();
        consume(TokenType::DOS_PUNTOS, "Se esperaba ':' despues de 'fecha_limite'");
        
        if (check(TokenType::FECHA)) {
            node->addChild(std::make_unique<ASTNode>(currentToken));
            advance();
        } else {
            hasSyntacticError = true;
            errors.addError(currentToken.getLexeme(), ErrorType::SINTACTICO_ERROR,
                           "Se esperaba una fecha en formato AAAA-MM-DD",
                           currentToken.getLine(), currentToken.getColumn(),
                           ErrorSeverity::ERROR_NORMAL);
        }
    }
    else {
        hasSyntacticError = true;
        errors.addError(currentToken.getLexeme(), ErrorType::SINTACTICO_ERROR,
                       "Se esperaba 'prioridad', 'responsable' o 'fecha_limite'",
                       currentToken.getLine(), currentToken.getColumn(),
                       ErrorSeverity::ERROR_NORMAL);
        synchronizeToNextValid();
    }
    
    return node;
}

std::unique_ptr<ASTNode> SyntaxAnalyzer::parseAtributos() {
    auto node = std::make_unique<ASTNode>("atributos");
    
    node->addChild(parseAtributo());
    
    while (check(TokenType::COMA)) {
        node->addChild(std::make_unique<ASTNode>(currentToken));
        advance();
        node->addChild(parseAtributo());
    }
    
    return node;
}

std::unique_ptr<ASTNode> SyntaxAnalyzer::parseTarea() {
    auto node = std::make_unique<ASTNode>("tarea");
    
    consume(TokenType::TAREA, "Se esperaba la palabra reservada 'tarea'");
    node->addChild(std::make_unique<ASTNode>(currentToken));
    
    consume(TokenType::DOS_PUNTOS, "Se esperaba ':' despues de 'tarea'");
    
    if (check(TokenType::CADENA)) {
        node->addChild(std::make_unique<ASTNode>(currentToken));
        advance();
    } else {
        hasSyntacticError = true;
        errors.addError(currentToken.getLexeme(), ErrorType::SINTACTICO_ERROR,
                       "Se esperaba el nombre de la tarea entre comillas",
                       currentToken.getLine(), currentToken.getColumn(),
                       ErrorSeverity::ERROR_NORMAL);
    }
    
    if (check(TokenType::CORCHETE_ABRE)) {
        node->addChild(std::make_unique<ASTNode>(currentToken));
        advance();
        node->addChild(parseAtributos());
        consume(TokenType::CORCHETE_CIERRA, "Se esperaba ']' para cerrar los atributos");
    }
    
    return node;
}

std::unique_ptr<ASTNode> SyntaxAnalyzer::parseTareas() {
    auto node = std::make_unique<ASTNode>("tareas");
    
    node->addChild(parseTarea());
    
    while (check(TokenType::COMA)) {
        node->addChild(std::make_unique<ASTNode>(currentToken));
        advance();
        node->addChild(parseTarea());
    }
    
    return node;
}

/*std::unique_ptr<ASTNode> SyntaxAnalyzer::parseTareas() {
    auto node = std::make_unique<ASTNode>("tareas");
    
    // Al menos una tarea
    node->addChild(parseTarea());
    
    // Mientras haya coma, seguimos
    while (check(TokenType::COMA)) {
        advance();
        // Permitir coma trailing: si no hay más tareas, salimos
        if (!check(TokenType::TAREA)) {
            break;  // Coma trailing permitida
        }
        node->addChild(parseTarea());
    }
    
    return node;
}*/

std::unique_ptr<ASTNode> SyntaxAnalyzer::parseColumna() {
    auto node = std::make_unique<ASTNode>("columna");
    
    consume(TokenType::COLUMNA, "Se esperaba la palabra reservada 'COLUMNA'");
    node->addChild(std::make_unique<ASTNode>(currentToken));
    
    if (check(TokenType::CADENA)) {
        node->addChild(std::make_unique<ASTNode>(currentToken));
        advance();
    } else {
        hasSyntacticError = true;
        errors.addError(currentToken.getLexeme(), ErrorType::SINTACTICO_ERROR,
                       "Se esperaba el nombre de la columna entre comillas",
                       currentToken.getLine(), currentToken.getColumn(),
                       ErrorSeverity::ERROR_NORMAL);
    }
    
    consume(TokenType::LLAVE_ABRE, "Se esperaba '{' para abrir el bloque de la columna");
    
    if (check(TokenType::TAREA)) {
        node->addChild(parseTareas());
    } else {
        auto emptyTareas = std::make_unique<ASTNode>("tareas_vacio");
        node->addChild(std::move(emptyTareas));
    }
    
    consume(TokenType::LLAVE_CIERRA, "Se esperaba '}' para cerrar el bloque de la columna");
    consume(TokenType::PUNTO_Y_COMA, "Se esperaba ';' al final de la columna");
    
    return node;
}

std::unique_ptr<ASTNode> SyntaxAnalyzer::parseColumnas() {
    auto node = std::make_unique<ASTNode>("columnas");
    
    node->addChild(parseColumna());
    
    while (check(TokenType::COLUMNA)) {
        node->addChild(parseColumna());
    }
    
    return node;
}

std::unique_ptr<ASTNode> SyntaxAnalyzer::parsePrograma() {
    auto node = std::make_unique<ASTNode>("programa");
    
    consume(TokenType::TABLERO, "Se esperaba 'TABLERO' al inicio del archivo");
    node->addChild(std::make_unique<ASTNode>(currentToken));
    
    if (check(TokenType::CADENA)) {
        node->addChild(std::make_unique<ASTNode>(currentToken));
        advance();
    } else {
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

std::unique_ptr<ASTNode> SyntaxAnalyzer::parse() {
    reset();
    
    currentToken = lexer.getNextToken();
    
    if (currentToken.getType() == TokenType::FIN_DE_ARCHIVO) {
        hasSyntacticError = true;
        errors.addError("", ErrorType::SINTACTICO_ERROR,
                       "El archivo esta vacio o no se pudieron leer tokens",
                       0, 0, ErrorSeverity::CRITICO_ERROR);
        return nullptr;
    }
    
    root = parsePrograma();
    
    if (currentToken.getType() != TokenType::FIN_DE_ARCHIVO) {
        hasSyntacticError = true;
        errors.addError(currentToken.getLexeme(), ErrorType::SINTACTICO_ERROR,
                       "Tokens adicionales despues del final esperado del programa",
                       currentToken.getLine(), currentToken.getColumn(),
                       ErrorSeverity::ERROR_NORMAL);
    }
    
    if (hasSyntacticError) {
        return nullptr;
    }
    
    // Retornar una copia (no mover) para mantener root en el parser
    return std::unique_ptr<ASTNode>(root.get());
}

bool SyntaxAnalyzer::isSuccessful() const {
    return !hasSyntacticError && root != nullptr;
}

std::string SyntaxAnalyzer::generateDotCode() const {
    std::stringstream ss;
    
    if (root == nullptr) {
        ss << "digraph ArbolDerivacion {\n";
        ss << "  label=\"No se pudo generar el arbol por errores sintacticos\";\n";
        ss << "  labelloc=t;\n";
        ss << "  fontsize=14;\n";
        ss << "}\n";
        return ss.str();
    }
    
    // Resetear el contador de nodos
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

void SyntaxAnalyzer::reset() {
    hasSyntacticError = false;
    root = nullptr;
}

} // namespace TaskScript