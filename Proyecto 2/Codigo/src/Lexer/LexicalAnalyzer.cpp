#include "LexicalAnalyzer.h"
#include <cctype>
#include <algorithm>

using namespace std;

namespace TaskScript
{

    LexicalAnalyzer::LexicalAnalyzer(ErrorManager &errorManager)
        : currentPos(0), line(1), column(1), errors(errorManager) {}

    bool LexicalAnalyzer::loadFile(const string &filename)
    {
        ifstream file(filename);
        if (!file.is_open())
        {
            errors.addError(filename, ErrorType::LEXICO,
                            "No se pudo abrir el archivo" + filename,
                            0, 0, ErrorSeverity::CRITICO);
            return false;
        }

        // Leer todo el contenido del archivo
        sourceCode = string((istreambuf_iterator<char>(file)),
                            istreambuf_iterator<char>());

        file.close();

        this->filename = filename;
        reset(); // Reiniciar el analizador para el nuevo archivo
        return true;
    }
    void LexicalAnalyzer::reset()
    {
        currentPos = 0;
        line = 1;
        column = 1;
        /*
        tokens.clear();*/   // No se limpian los tokens para permitir generar reportes incluso si hay errores
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
            if (c == ' ' || c == '\t' || c == '\r')
            {
                advance(); // Omitir espacios, tabs y retornos de carro
            }
            else if (c == '\n')
            {
                advance(); // Avanzar y actualizar posición para nueva línea
            }
            else if (c == '/' && peekNext() == '/')
            {
                // Comentario de una línea, avanzar hasta el final de la línea
                while (!isAtEnd() && peek() != '\n')
                    advance();
            }
            else
            {
                break; // No es espacio ni comentario, salir del bucle
            }
        }
    }

    Token LexicalAnalyzer::readString()
    {
        int startLine = line;
        int startColumn = column;
        string lexeme;

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
        // Si llegamos al final sin cerrar la cadena
        string errorLexeme = "\"" + lexeme;
        errors.addError(errorLexeme, ErrorType::LEXICO,
                        "Cadena no cerrada antes del fin de archivo. Se esperaba \"\"",
                        startLine, startColumn, ErrorSeverity::CRITICO);

        Token token(TokenType::DESCONOCIDO, errorLexeme, startLine, startColumn);
        tokens.push_back(token);
        return token;
    }
    Token LexicalAnalyzer::readNumberOrDate()
    {
        int startLine = line;
        int startColumn = column;
        std::string lexeme;

        // Leer digitos iniciales
        while (!isAtEnd() && std::isdigit(peek()))
        {
            lexeme += advance();
        }

        // Verificar si es una fecha (formato AAAA-MM-DD)
        if (lexeme.length() == 4 && peek() == '-')
        {
            lexeme += advance(); // Agrega el guion

            // Leer dos digitos del mes
            if (!isAtEnd() && std::isdigit(peek()))
            {
                lexeme += advance();
                if (!isAtEnd() && std::isdigit(peek()))
                {
                    lexeme += advance();

                    // Verificar el segundo guion
                    if (peek() == '-')
                    {
                        lexeme += advance();

                        // Leer dos digitos del dia
                        if (!isAtEnd() && std::isdigit(peek()))
                        {
                            lexeme += advance();
                            if (!isAtEnd() && std::isdigit(peek()))
                            {
                                lexeme += advance();

                                // Validacion basica del formato
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

            // Si no cumplio el formato de fecha, reportar error
            errors.addError(lexeme, ErrorType::LEXICO,
                            "Formato de fecha invalido. Se esperaba AAAA-MM-DD",
                            startLine, startColumn);
        }

        // Si no es fecha, es un numero entero
        Token token(TokenType::ENTERO, lexeme, startLine, startColumn);
        tokens.push_back(token);
        return token;
    }

    TokenType LexicalAnalyzer::getKeywordType(const string &word) const
    {
        // Mapeo de palabras reservadas a sus tipos de token
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

        // Si no es palabra reservada, no es valida en este lenguaje
        return TokenType::DESCONOCIDO;
    }

    Token LexicalAnalyzer::readKeywordOrIdentifier()
    {
        int startLine = line;
        int startColumn = column;
        std::string lexeme;

        // Leer mientras sean caracteres alfanumericos o guion bajo
        while (!isAtEnd() && (std::isalnum(peek()) || peek() == '_'))
        {
            lexeme += advance();
        }

        TokenType type = getKeywordType(lexeme);

        // Si no es una palabra reservada, es un identificador invalido
        if (type == TokenType::DESCONOCIDO)
        {
            errors.addError(lexeme, ErrorType::LEXICO,
                            "Caracter o palabra no reconocida. No se permiten identificadores sueltos",
                            startLine, startColumn);
        }

        Token token(type, lexeme, startLine, startColumn);
        tokens.push_back(token);
        return token;
    }
    Token LexicalAnalyzer::readSymbol()
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
            errors.addError(lexeme, ErrorType::LEXICO,
                            "Caracter no reconocido: '" + std::string(1, c) + "'",
                            startLine, startColumn);
            break;
        }
        Token token(type, lexeme, startLine, startColumn);
        tokens.push_back(token);
        return token;
    }
    Token LexicalAnalyzer::getNextToken()
    {
        // Saltar espacios en blanco antes de comenzar
        skipWhitespace();

        if (isAtEnd())
        {
            Token eofToken(TokenType::FIN_DE_ARCHIVO, "", line, column);
            return eofToken;
        }

        char current = peek();

        // Implementacion del AFD mediante despacho a metodos especificos
        if (current == '"')
        {
            return readString();
        }
        else if (std::isdigit(current))
        {
            return readNumberOrDate();
        }
        else if (std::isalpha(current))
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