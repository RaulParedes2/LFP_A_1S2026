#include "Token.h"

using namespace std;

namespace TaskScript
{
    Token::Token()
    : type(TokenType::DESCONOCIDO), lexeme(""), line(0), column(0) {}
    
    Token::Token(TokenType type, const string &lexeme, int line, int column)
        : type(type), lexeme(lexeme), line(line), column(column) {}

    TokenType Token::getType() const
    {
        return type;
    }

    string Token::getLexeme() const
    {
        return lexeme;
    }

    int Token::getLine() const
    {
        return line;
    }

    int Token::getColumn() const
    {
        return column;
    }

    string Token::typeToString() const
    {
        switch (type)
        {
        case TokenType::TABLERO:
            return "TABLERO";
        case TokenType::COLUMNA:
            return "COLUMNA";
        case TokenType::TAREA:
            return "TAREA";
        case TokenType::PRIORIDAD:
            return "PRIORIDAD";
        case TokenType::RESPONSABLE:
            return "RESPONSABLE";
        case TokenType::FECHA_LIMITE:
            return "FECHA_LIMITE";
        case TokenType::ALTA:
            return "ALTA";
        case TokenType::MEDIA:
            return "MEDIA";
        case TokenType::BAJA:
            return "BAJA";
        case TokenType::IDENTIFICADOR:
            return "IDENTIFICADOR";
        case TokenType::CADENA:
            return "CADENA";
        case TokenType::FECHA:
            return "FECHA";
        case TokenType::ENTERO:
            return "ENTERO";
        case TokenType::LLAVE_ABRE:
            return "LLAVE_ABRE";
        case TokenType::LLAVE_CIERRA:
            return "LLAVE_CIERRA";
        case TokenType::CORCHETE_ABRE:
            return "CORCHETE_ABRE";
        case TokenType::CORCHETE_CIERRA:
            return "CORCHETE_CIERRA";
        case TokenType::DOS_PUNTOS:
            return "DOS_PUNTOS";
        case TokenType::COMA:
            return "COMA";
        case TokenType::PUNTO_Y_COMA:
            return "PUNTO_Y_COMA";
        case TokenType::FIN_DE_ARCHIVO:
            return "FIN_DE_ARCHIVO";
        case TokenType::DESCONOCIDO:
            return "DESCONOCIDO";
        default:
            return "INDEFINIDO";
        }
    }

    string Token::toString() const
    {
        return "Token[" + typeToString() + ", \"" + lexeme + "\", " +
               std::to_string(line) + ":" + std::to_string(column) + "]";
    }
}