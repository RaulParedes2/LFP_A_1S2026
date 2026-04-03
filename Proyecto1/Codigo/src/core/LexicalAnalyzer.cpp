#include "../../include/core/LexicalAnalyzer.h"
#include <cctype>
#include <sstream>
#include <iostream>

using namespace std;

LexicalAnalyzer::LexicalAnalyzer(ErrorManager *errorMgr)
    : errorManager(errorMgr), position(0), line(1), column(1), currentChar(' ') {}

LexicalAnalyzer::~LexicalAnalyzer() {}

bool LexicalAnalyzer::loadFile(const string &filename)
{
    ifstream file(filename);
    if (!file.is_open())
    {
        return false;
    }

    stringstream buffer;
    buffer << file.rdbuf();
    sourceCode = buffer.str();
    file.close();

    position = 0;
    line = 1;
    column = 1;
    tokens.clear();

    return true;
}

void LexicalAnalyzer::setSourceCode(const string &source)
{
    sourceCode = source;
    position = 0;
    line = 1;
    column = 1;
    tokens.clear();
}

void LexicalAnalyzer::advance()
{
    if (position < sourceCode.length())
    {
        currentChar = sourceCode[position];
        position++;
        column++;

        if (currentChar == '\n')
        {
            line++;
            column = 1;
        }
    }
    else
    {
        currentChar = '\0';
    }
}

char LexicalAnalyzer::peek() const
{
    if (position < sourceCode.length())
    {
        return sourceCode[position];
    }
    return '\0';
}

bool LexicalAnalyzer::isEOF() const
{
    return position >= sourceCode.length();
}

void LexicalAnalyzer::skipWhitespace()
{
    while (!isEOF() && isspace(currentChar))
    {
        advance();
    }
}

vector<Token> LexicalAnalyzer::analyze()
{
    tokens.clear();
    position = 0;
    line = 1;
    column = 1;

    if (!sourceCode.empty())
    {
        advance();
    }

    while (!isEOF())
    {
        skipWhitespace();

        if (isEOF())
            break;

        Token token = nextToken();
        if (token.getType() != TokenType::ERROR)
        {
            tokens.push_back(token);
        }
    }

    tokens.push_back(Token(TokenType::END_OF_FILE, "", line, column));

    return tokens;
}

Token LexicalAnalyzer::nextToken()
{
    int startLine = line;
    int startColumn = column;

    if (isalpha(currentChar))
    {
        return recognizeIdentifier();
    }
    else if (isdigit(currentChar))
    {
        return recognizeNumberOrDateTime();
    }
    else if (currentChar == '"')
    {
        return recognizeString();
    }
    else
    {
        char c = currentChar;
        string lexeme(1, c);
        advance();

        switch (c)
        {
        case '{':
            return Token(TokenType::LBRACE, lexeme, startLine, startColumn);
        case '}':
            return Token(TokenType::RBRACE, lexeme, startLine, startColumn);
        case '[':
            return Token(TokenType::LBRACKET, lexeme, startLine, startColumn);
        case ']':
            return Token(TokenType::RBRACKET, lexeme, startLine, startColumn);
        case ':':
            return Token(TokenType::COLON, lexeme, startLine, startColumn);
        case ',':
            return Token(TokenType::COMMA, lexeme, startLine, startColumn);
        case ';':
            return Token(TokenType::SEMICOLON, lexeme, startLine, startColumn);
        case '-':
            return Token(TokenType::HYPHEN, lexeme, startLine, startColumn);
        default:
            if (errorManager)
            {
                errorManager->addError(lexeme, "Caracter ilegal",
                                       "El caracter '" + string(1, c) + "' no es valido en el lenguaje",
                                       startLine, startColumn, ErrorSeverity::ERROR);
            }
            return Token(TokenType::ERROR, lexeme, startLine, startColumn);
        }
    }
}

Token LexicalAnalyzer::recognizeNumberOrDateTime()
{
    int startLine = line;
    int startColumn = column;
    string lexeme;

    while (!isEOF() && isdigit(currentChar))
    {
        lexeme += currentChar;
        advance();
    }

    // Verificar fecha (YYYY-MM-DD) sin regex
    if (currentChar == '-' && isdigit(peek()))
    {
        lexeme += currentChar;
        advance();

        while (!isEOF() && isdigit(currentChar))
        {
            lexeme += currentChar;
            advance();
        }

        if (currentChar == '-' && isdigit(peek()))
        {
            lexeme += currentChar;
            advance();

            while (!isEOF() && isdigit(currentChar))
            {
                lexeme += currentChar;
                advance();
            }

            if (isValidDate(lexeme))
            {
                return Token(TokenType::DATE, lexeme, startLine, startColumn);
            }
            else
            {
                if (errorManager)
                {
                    errorManager->addError(lexeme, "Fecha invalida",
                                           "La fecha '" + lexeme + "' no es valida. Formato esperado: AAAA-MM-DD",
                                           startLine, startColumn, ErrorSeverity::ERROR);
                }
                return Token(TokenType::ERROR, lexeme, startLine, startColumn);
            }
        }
    }

    // Verificar hora (HH:MM) sin regex
    if (currentChar == ':' && isdigit(peek()))
    {
        lexeme += currentChar;
        advance();

        while (!isEOF() && isdigit(currentChar))
        {
            lexeme += currentChar;
            advance();
        }

        if (isValidTime(lexeme))
        {
            return Token(TokenType::TIME, lexeme, startLine, startColumn);
        }
        else
        {
            if (errorManager)
            {
                errorManager->addError(lexeme, "Hora invalida",
                                       "La hora '" + lexeme + "' no es valida. Formato esperado: HH:MM",
                                       startLine, startColumn, ErrorSeverity::ERROR);
            }
            return Token(TokenType::ERROR, lexeme, startLine, startColumn);
        }
    }

    return Token(TokenType::INTEGER, lexeme, startLine, startColumn);
}

Token LexicalAnalyzer::recognizeIdentifier()
{
    int startLine = line;
    int startColumn = column;
    string lexeme;

    while (!isEOF() && (isalnum(currentChar) || currentChar == '_'))
    {
        lexeme += currentChar;
        advance();
    }

    // Palabras reservadas
    if (lexeme == "HOSPITAL")
        return Token(TokenType::HOSPITAL, lexeme, startLine, startColumn);
    if (lexeme == "PACIENTES")
        return Token(TokenType::PACIENTES, lexeme, startLine, startColumn);
    if (lexeme == "MEDICOS")
        return Token(TokenType::MEDICOS, lexeme, startLine, startColumn);
    if (lexeme == "CITAS")
        return Token(TokenType::CITAS, lexeme, startLine, startColumn);
    if (lexeme == "DIAGNOSTICOS")
        return Token(TokenType::DIAGNOSTICOS, lexeme, startLine, startColumn);
    if (lexeme == "paciente")
        return Token(TokenType::PACIENTE, lexeme, startLine, startColumn);
    if (lexeme == "medico")
        return Token(TokenType::MEDICO, lexeme, startLine, startColumn);
    if (lexeme == "cita")
        return Token(TokenType::CITA, lexeme, startLine, startColumn);
    if (lexeme == "diagnostico")
        return Token(TokenType::DIAGNOSTICO, lexeme, startLine, startColumn);
    if (lexeme == "con")
        return Token(TokenType::CON, lexeme, startLine, startColumn);

    // Atributos
    if (lexeme == "edad" || lexeme == "tipo_sangre" || lexeme == "especialidad" ||
        lexeme == "codigo" || lexeme == "fecha" || lexeme == "hora" ||
        lexeme == "condicion" || lexeme == "medicamento" || lexeme == "dosis")
    {
        return Token(TokenType::ATRIBUTO, lexeme, startLine, startColumn);
    }

    if (isValidSpecialty(lexeme))
    {
        return Token(TokenType::ESPECIALIDAD, lexeme, startLine, startColumn);
    }

    if (isValidDosis(lexeme))
    {
        return Token(TokenType::DOSIS, lexeme, startLine, startColumn);
    }

    if (isValidBloodType(lexeme))
    {
        return Token(TokenType::BLOOD_TYPE, lexeme, startLine, startColumn);
    }

    if (errorManager)
    {
        errorManager->addError(lexeme, "Token invalido",
                               "El identificador '" + lexeme + "' no es reconocido",
                               startLine, startColumn, ErrorSeverity::ERROR);
    }
    return Token(TokenType::ERROR, lexeme, startLine, startColumn);
}

Token LexicalAnalyzer::recognizeNumber()
{
    int startLine = line;
    int startColumn = column;
    string lexeme;

    while (!isEOF() && isdigit(currentChar))
    {
        lexeme += currentChar;
        advance();
    }

    return Token(TokenType::INTEGER, lexeme, startLine, startColumn);
}

Token LexicalAnalyzer::recognizeString()
{
    int startLine = line;
    int startColumn = column;
    string lexeme;

    advance(); // Saltar comilla inicial

    while (!isEOF() && currentChar != '"')
    {
        lexeme += currentChar;
        advance();
    }

    if (currentChar == '"')
    {
        advance(); // Saltar comilla de cierre

        // VALIDAR TIPO DE SANGRE
        if (lexeme == "A+" || lexeme == "A-" || lexeme == "B+" || lexeme == "B-" ||
            lexeme == "O+" || lexeme == "O-" || lexeme == "AB+" || lexeme == "AB-")
        {
            return Token(TokenType::BLOOD_TYPE, lexeme, startLine, startColumn);
        }

        // VALIDAR QUE NO SEA UN TIPO DE SANGRE INVÁLIDO
        // Si tiene formato de tipo de sangre (una letra seguida de + o -) pero es inválido
        if ((lexeme.length() == 2 || lexeme.length() == 3) &&
            (lexeme.find('+') != string::npos || lexeme.find('-') != string::npos))
        {
            errorManager->addError(lexeme, "Tipo de sangre invalido",
                                   "El valor '" + lexeme + "' no es un tipo de sangre valido. Valores validos: O+, O-, A+, A-, B+, B-, AB+, AB-",
                                   startLine, startColumn, ErrorSeverity::ERROR);
            return Token(TokenType::ERROR, lexeme, startLine, startColumn);
        }

        return Token(TokenType::STRING, lexeme, startLine, startColumn);
    }
    else
    {
        errorManager->addError(lexeme, "Cadena sin cerrar",
                               "Se encontro el inicio de una cadena pero no se detecto el cierre",
                               startLine, startColumn, ErrorSeverity::CRITICAL);
        return Token(TokenType::ERROR, lexeme, startLine, startColumn);
    }
}
bool LexicalAnalyzer::isValidDate(const string &dateStr)
{
    // Validación manual sin regex
    if (dateStr.length() != 10)
        return false;
    if (dateStr[4] != '-' || dateStr[7] != '-')
        return false;

    for (int i = 0; i < 10; i++)
    {
        if (i == 4 || i == 7)
            continue;
        if (!isdigit(dateStr[i]))
            return false;
    }

    int year = stoi(dateStr.substr(0, 4));
    int month = stoi(dateStr.substr(5, 2));
    int day = stoi(dateStr.substr(8, 2));

    if (month < 1 || month > 12)
        return false;

    int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (month == 2)
    {
        bool isLeap = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
        if (isLeap)
            daysInMonth[1] = 29;
    }

    return day >= 1 && day <= daysInMonth[month - 1];
}

bool LexicalAnalyzer::isValidTime(const string &timeStr)
{
    // Validación manual sin regex
    if (timeStr.length() != 5)
        return false;
    if (timeStr[2] != ':')
        return false;

    for (int i = 0; i < 5; i++)
    {
        if (i == 2)
            continue;
        if (!isdigit(timeStr[i]))
            return false;
    }

    int hour = stoi(timeStr.substr(0, 2));
    int minute = stoi(timeStr.substr(3, 2));

    return hour >= 0 && hour <= 23 && minute >= 0 && minute <= 59;
}

bool LexicalAnalyzer::isValidBloodType(const string &bloodType)
{
    vector<string> validTypes = {"A+", "A-", "B+", "B-", "O+", "O-", "AB+", "AB-"};
    for (const auto &type : validTypes)
    {
        if (bloodType == type)
            return true;
    }
    return false;
}

bool LexicalAnalyzer::isValidSpecialty(const string &specialty)
{
    vector<string> validSpecialties = {
        "CARDIOLOGIA", "NEUROLOGIA", "PEDIATRIA", "CIRUGIA",
        "MEDICINA_GENERAL", "ONCOLOGIA"};
    for (const auto &spec : validSpecialties)
    {
        if (specialty == spec)
            return true;
    }
    return false;
}

bool LexicalAnalyzer::isValidDosis(const string &dosis)
{
    vector<string> validDosis = {
        "DIARIA", "CADA_8_HORAS", "CADA_12_HORAS", "SEMANAL"};
    for (const auto &d : validDosis)
    {
        if (dosis == d)
            return true;
    }
    return false;
}

void LexicalAnalyzer::printTokens() const
{
    cout << "\n=== TOKENS ===" << endl;
    for (const auto &token : tokens)
    {
        token.print();
    }
}
