#ifndef LEXICAL_ANALYZER_H
#define LEXICAL_ANALYZER_H

#include <string>
#include <vector>
#include <fstream>
#include "Token.h"
#include "ErrorManager.h"

class LexicalAnalyzer {
private:
    std::string sourceCode;
    std::vector<Token> tokens;
    ErrorManager* errorManager;
    
    // Posición actual en el análisis
    int position;
    int line;
    int column;
    
    // Caracter actual
    char currentChar;
    
    // Métodos auxiliares
    void advance();
    void skipWhitespace();
    char peek() const;
    bool isEOF() const;
    
    // Métodos de reconocimiento
    Token recognizeIdentifier();
    Token recognizeNumber();
    Token recognizeNumberOrDateTime();
    Token recognizeString();
    
    // Validadores específicos
    bool isValidDate(const std::string& dateStr);
    bool isValidTime(const std::string& timeStr);
    bool isValidBloodType(const std::string& bloodType);
    bool isValidSpecialty(const std::string& specialty);
    bool isValidDosis(const std::string& dosis);
    
public:
    LexicalAnalyzer(ErrorManager* errorMgr);
    ~LexicalAnalyzer();
    
    // Cargar archivo fuente
    bool loadFile(const std::string& filename);
    void setSourceCode(const std::string& source);
    
    // Análisis léxico principal
    std::vector<Token> analyze();
    Token nextToken();
    
    // Getters
    const std::vector<Token>& getTokens() const { return tokens; }
    
    // Reporte de tokens
    void printTokens() const;
};

#endif