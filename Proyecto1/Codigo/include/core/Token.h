#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include <iostream>

enum class TokenType {
    // Palabras reservadas
    HOSPITAL,
    PACIENTES,
    MEDICOS,
    CITAS,
    DIAGNOSTICOS,
    PACIENTE,
    MEDICO,
    CITA,
    DIAGNOSTICO,
    
    // Delimitadores
    LBRACE,      // {
    RBRACE,      // }
    LBRACKET,    // [
    RBRACKET,    // ]
    COLON,       // :
    COMMA,       // ,
    SEMICOLON,   // ;
    
    // Literales y valores
    STRING,      // "texto"
    INTEGER,     // 123
    DATE,        // 2024-03-23
    TIME,        // 14:30
    CODE_ID,     // MED-001
    
    // Enumeraciones médicas
    ESPECIALIDAD,
    DOSIS,
    BLOOD_TYPE,
    
    // Error
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
    
    // Getters
    TokenType getType() const;
    std::string getLexeme() const;
    int getLine() const;
    int getColumn() const;
    
    // Convertir tipo a string para reportes
    static std::string tokenTypeToString(TokenType type);
    
    // Mostrar token (para debug)
    void print() const;
};

#endif