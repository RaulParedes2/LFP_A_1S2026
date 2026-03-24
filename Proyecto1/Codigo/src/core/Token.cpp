#include "../../include/core/Token.h"

Token::Token() : type(TokenType::ERROR), lexeme(""), line(0), column(0) {}

Token::Token(TokenType type, const std::string& lexeme, int line, int column)
    : type(type), lexeme(lexeme), line(line), column(column) {}

TokenType Token::getType() const {
    return type;
}

std::string Token::getLexeme() const {
    return lexeme;
}

int Token::getLine() const {
    return line;
}

int Token::getColumn() const {
    return column;
}

std::string Token::tokenTypeToString(TokenType type) {
    switch(type) {
        case TokenType::HOSPITAL: return "HOSPITAL";
        case TokenType::PACIENTES: return "PACIENTES";
        case TokenType::MEDICOS: return "MEDICOS";
        case TokenType::CITAS: return "CITAS";
        case TokenType::DIAGNOSTICOS: return "DIAGNOSTICOS";
        case TokenType::PACIENTE: return "PACIENTE";
        case TokenType::MEDICO: return "MEDICO";
        case TokenType::CITA: return "CITA";
        case TokenType::DIAGNOSTICO: return "DIAGNOSTICO";
        case TokenType::LBRACE: return "{";
        case TokenType::RBRACE: return "}";
        case TokenType::LBRACKET: return "[";
        case TokenType::RBRACKET: return "]";
        case TokenType::COLON: return ":";
        case TokenType::COMMA: return ",";
        case TokenType::SEMICOLON: return ";";
        case TokenType::STRING: return "STRING";
        case TokenType::INTEGER: return "INTEGER";
        case TokenType::DATE: return "DATE";
        case TokenType::TIME: return "TIME";
        case TokenType::CODE_ID: return "CODE_ID";
        case TokenType::ESPECIALIDAD: return "ESPECIALIDAD";
        case TokenType::DOSIS: return "DOSIS";
        case TokenType::BLOOD_TYPE: return "BLOOD_TYPE";
        case TokenType::ERROR: return "ERROR";
        case TokenType::END_OF_FILE: return "EOF";
        default: return "UNKNOWN";
    }
}

void Token::print() const {
    std::cout << "Token{ type: " << tokenTypeToString(type)
              << ", lexeme: '" << lexeme << "'"
              << ", line: " << line
              << ", col: " << column << " }" << std::endl;
}