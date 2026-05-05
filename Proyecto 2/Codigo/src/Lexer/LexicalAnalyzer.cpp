#include "LexicalAnalyzer.h"
#include <cctype>
#include <algorithm>
#include <fstream>
#include <iostream>

namespace TaskScript
{

    LexicalAnalyzer::LexicalAnalyzer(ErrorManager &errorManager)
        : currentPos(0), line(1), column(1), errors(errorManager) {}

    bool LexicalAnalyzer::loadFile(const std::string &filename)
    {
        std::ifstream file(filename, std::ios::binary);

        if (!file.is_open())
        {
            errors.addError(filename, ErrorType::LEXICO_ERROR,
                            "No se pudo abrir el archivo: " + filename,
                            0, 0, ErrorSeverity::CRITICO_ERROR);
            return false;
        }

        // Leer todo el contenido
        sourceCode = std::string((std::istreambuf_iterator<char>(file)),
                                 std::istreambuf_iterator<char>());
        file.close();

        // Eliminar BOM de UTF-8 si existe
        if (sourceCode.length() >= 3 &&
            (unsigned char)sourceCode[0] == 0xEF &&
            (unsigned char)sourceCode[1] == 0xBB &&
            (unsigned char)sourceCode[2] == 0xBF)
        {
            sourceCode = sourceCode.substr(3);
            std::cout << "BOM eliminado" << std::endl;
        }

        // Normalizar saltos de linea: CRLF -> LF, CR -> LF
        std::string normalized;
        for (size_t i = 0; i < sourceCode.length(); i++)
        {
            if (sourceCode[i] == '\r')
            {
                normalized += '\n';
                if (i + 1 < sourceCode.length() && sourceCode[i + 1] == '\n')
                {
                    i++;
                }
            }
            else
            {
                normalized += sourceCode[i];
            }
        }
        sourceCode = normalized;

        this->filename = filename;
        reset();

        std::cout << "Archivo cargado: " << sourceCode.length() << " caracteres" << std::endl;
        return true;
    }

    void LexicalAnalyzer::reset()
    {
        currentPos = 0;
        line = 1;
        column = 1;
        tokens.clear();
    }

    char LexicalAnalyzer::peek() const
    {
        if (isAtEnd())
            return '\0';
        return sourceCode[currentPos];
    }

    char LexicalAnalyzer::peekNext() const
    {
        if (currentPos + 1 >= sourceCode.length())
            return '\0';
        return sourceCode[currentPos + 1];
    }

    char LexicalAnalyzer::advance()
    {
        if (isAtEnd())
            return '\0';
        char c = sourceCode[currentPos++];
        updatePosition(c);
        return c;
    }

    bool LexicalAnalyzer::isAtEnd() const
    {
        return currentPos >= sourceCode.length();
    }

    void LexicalAnalyzer::updatePosition(char c)
    {
        if (c == '\n')
        {
            line++;
            column = 1;
        }
        else
        {
            column++;
        }
    }

    void LexicalAnalyzer::skipWhitespace()
    {
        while (!isAtEnd())
        {
            char c = peek();
            if (c == ' ' || c == '\t')
            {
                advance();
            }
            else if (c == '\n')
            {
                advance();
            }
            else
            {
                break;
            }
        }
    }

    Token LexicalAnalyzer::readString()
    {
        int startLine = line;
        int startColumn = column;
        std::string lexeme;

        advance(); // Consume la comilla de apertura

        while (!isAtEnd())
        {
            char c = peek();

            if (c == '"')
            {
                advance(); // Consume la comilla de cierre
                Token token(TokenType::CADENA, lexeme, startLine, startColumn);
                tokens.push_back(token);
                return token;
            }

            lexeme += c;
            advance();
        }

        // ERROR: Cadena no cerrada
        std::string errorLexeme = "\"" + lexeme;
        errors.addError(errorLexeme, ErrorType::LEXICO_ERROR,
                        "Cadena no cerrada antes del fin de archivo",
                        startLine, startColumn, ErrorSeverity::CRITICO_ERROR);

        // Crear token de error
        Token token(TokenType::DESCONOCIDO, errorLexeme, startLine, startColumn);
        tokens.push_back(token);
        return token;
    }

    Token LexicalAnalyzer::readNumberOrDate()
    {
        int startLine = line;
        int startColumn = column;
        std::string lexeme;

        while (!isAtEnd() && std::isdigit(static_cast<unsigned char>(peek())))
        {
            lexeme += advance();
        }

        // Verificar si es una fecha (formato AAAA-MM-DD)
        if (lexeme.length() == 4 && peek() == '-')
        {
            lexeme += advance(); // Agrega el guion

            // Leer mes
            if (!isAtEnd() && std::isdigit(static_cast<unsigned char>(peek())))
            {
                lexeme += advance();
                if (!isAtEnd() && std::isdigit(static_cast<unsigned char>(peek())))
                {
                    lexeme += advance();

                    // Verificar segundo guion
                    if (peek() == '-')
                    {
                        lexeme += advance();

                        // Leer dia
                        if (!isAtEnd() && std::isdigit(static_cast<unsigned char>(peek())))
                        {
                            lexeme += advance();
                            if (!isAtEnd() && std::isdigit(static_cast<unsigned char>(peek())))
                            {
                                lexeme += advance();

                                if (lexeme.length() == 10)
                                {
                                    Token token(TokenType::FECHA, lexeme, startLine, startColumn);
                                    tokens.push_back(token);
                                    return token;
                                }
                            }
                        }
                    }
                }
            }

            // Si no es fecha valida, es un error
            errors.addError(lexeme, ErrorType::LEXICO_ERROR,
                            "Formato de fecha invalido. Se esperaba AAAA-MM-DD",
                            startLine, startColumn);
        }

        Token token(TokenType::ENTERO, lexeme, startLine, startColumn);
        tokens.push_back(token);
        return token;
    }

    TokenType LexicalAnalyzer::getKeywordType(const std::string &word) const
    {
        if (word == "TABLERO")
            return TokenType::TABLERO;
        if (word == "COLUMNA")
            return TokenType::COLUMNA;
        if (word == "tarea")
            return TokenType::TAREA;
        if (word == "prioridad")
            return TokenType::PRIORIDAD;
        if (word == "responsable")
            return TokenType::RESPONSABLE;
        if (word == "fecha_limite")
            return TokenType::FECHA_LIMITE;
        if (word == "ALTA")
            return TokenType::ALTA;
        if (word == "MEDIA")
            return TokenType::MEDIA;
        if (word == "BAJA")
            return TokenType::BAJA;

        return TokenType::DESCONOCIDO;
    }

    Token LexicalAnalyzer::readKeywordOrIdentifier()
    {
        int startLine = line;
        int startColumn = column;
        std::string lexeme;

        while (!isAtEnd() && (std::isalnum(static_cast<unsigned char>(peek())) || peek() == '_'))
        {
            lexeme += advance();
        }

        TokenType type = getKeywordType(lexeme);

        if (type == TokenType::DESCONOCIDO)
        {
            // Solo reportar error si NO es una palabra reservada conocida
            // ALTA, MEDIA, BAJA ya son reconocidas, no deben dar error
            errors.addError(lexeme, ErrorType::LEXICO_ERROR,
                            "Palabra no reconocida: " + lexeme,
                            startLine, startColumn);
        }

        Token token(type, lexeme, startLine, startColumn);
        tokens.push_back(token);
        return token;
    }

    /*Token LexicalAnalyzer::readSymbol()
     {
         int startLine = line;
         int startColumn = column;
         char c = advance();
         std::string lexeme(1, c);
         TokenType type;

         switch (c)
         {
         case '{':
             type = TokenType::LLAVE_ABRE;
             break;
         case '}':
             type = TokenType::LLAVE_CIERRA;
             break;
         case '[':
             type = TokenType::CORCHETE_ABRE;
             break;
         case ']':
             type = TokenType::CORCHETE_CIERRA;
             break;
         case ':':
             type = TokenType::DOS_PUNTOS;
             break;
         case ',':
             type = TokenType::COMA;
             break;
         case ';':
             type = TokenType::PUNTO_Y_COMA;
             break;
         default:
             type = TokenType::DESCONOCIDO;
             errors.addError(lexeme, ErrorType::LEXICO_ERROR,
                             "Caracter no reconocido: '" + std::string(1, c) + "'",
                             startLine, startColumn);
             break;
         }

         Token token(type, lexeme, startLine, startColumn);
         tokens.push_back(token);
         return token;
     }*/

    Token LexicalAnalyzer::readSymbol() {
    int startLine = line;
    int startColumn = column;
    char c = advance();
    std::string lexeme(1, c);
    TokenType type;
    
    switch (c) {
        case '{': type = TokenType::LLAVE_ABRE; break;
        case '}': type = TokenType::LLAVE_CIERRA; break;
        case '[': type = TokenType::CORCHETE_ABRE; break;
        case ']': type = TokenType::CORCHETE_CIERRA; break;
        case ':': type = TokenType::DOS_PUNTOS; break;
        case ',': type = TokenType::COMA; break;
        case ';': type = TokenType::PUNTO_Y_COMA; break;
        default:
            type = TokenType::DESCONOCIDO;
            // Solo agregar error si no es un token repetido
            errors.addError(lexeme, ErrorType::LEXICO_ERROR,
                           "Caracter no reconocido: '" + std::string(1, c) + "'",
                           startLine, startColumn,
                           ErrorSeverity::ERROR_NORMAL);
            break;
    }
    
    Token token(type, lexeme, startLine, startColumn);
    tokens.push_back(token);
    return token;
}

    Token LexicalAnalyzer::getNextToken()
    {
        skipWhitespace();

        if (isAtEnd())
        {
            Token eofToken(TokenType::FIN_DE_ARCHIVO, "", line, column);
            return eofToken;
        }

        char current = peek();

        if (current == '"')
        {
            return readString();
        }
        else if (std::isdigit(static_cast<unsigned char>(current)))
        {
            return readNumberOrDate();
        }
        else if (std::isalpha(static_cast<unsigned char>(current)))
        {
            return readKeywordOrIdentifier();
        }
        else
        {
            return readSymbol();
        }
    }

    const std::vector<Token> &LexicalAnalyzer::getTokens() const
    {
        return tokens;
    }

    std::string LexicalAnalyzer::getStatistics() const
    {
        int tokenCount = tokens.size();
        int validTokens = 0;

        for (const auto &token : tokens)
        {
            if (token.getType() != TokenType::DESCONOCIDO &&
                token.getType() != TokenType::FIN_DE_ARCHIVO)
            {
                validTokens++;
            }
        }

        return "Archivo: " + filename + " | Tokens: " + std::to_string(tokenCount) +
               " | Validos: " + std::to_string(validTokens) +
               " | Invalidos: " + std::to_string(tokenCount - validTokens);
    }

} // namespace TaskScript