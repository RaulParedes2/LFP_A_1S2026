#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include <iostream>

enum class TokenType {
    HOSPITAL,
    PACIENTES,
    MEDICOS,
    CITAS,
    DIAGNOSTICOS,
    PACIENTE,
    MEDICO,
    CITA,
    DIAGNOSTICO,
    CON,              // NUEVO: palabra "con"
    ATRIBUTO,         // NUEVO: edad, tipo_sangre, especialidad, etc.
    LBRACE,
    RBRACE,
    LBRACKET,
    RBRACKET,
    COLON,
    COMMA,
    SEMICOLON,
    HYPHEN,           // NUEVO: guión -
    STRING,
    INTEGER,
    DATE,
    TIME,
    CODE_ID,
    ESPECIALIDAD,
    DOSIS,
    BLOOD_TYPE,
    ERROR,
    END_OF_FILE
};

class Token {
private:
    TokenType type;
    std::string lexeme;
    int line;
    int column;
    
public:
    Token();
    Token(TokenType type, const std::string& lexeme, int line, int column);
    
    TokenType getType() const;
    std::string getLexeme() const;
    int getLine() const;
    int getColumn() const;
    
    static std::string tokenTypeToString(TokenType type);
    void print() const;
};

#endif