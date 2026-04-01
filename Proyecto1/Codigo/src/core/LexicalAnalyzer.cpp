#include "../../include/core/LexicalAnalyzer.h"
#include <cctype>
#include <sstream>
#include <regex>

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
        advance(); // Cargar primer caracter
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

    // Agregar token EOF
    tokens.push_back(Token(TokenType::END_OF_FILE, "", line, column));

    return tokens;
}

Token LexicalAnalyzer::nextToken() {
    int startLine = line;
    int startColumn = column;
    
    // Identificar el tipo de token basado en el caracter actual
    if (isalpha(currentChar)) {
        return recognizeIdentifier();
    }
    else if (isdigit(currentChar)) {
        // Podría ser número, fecha u hora
        return recognizeNumberOrDateTime();
    }
    else if (currentChar == '"') {
        return recognizeString();
    }
    else {
        // Delimitadores y símbolos
        char c = currentChar;
        string lexeme(1, c);
        advance();
        
        switch (c) {
            case '{': return Token(TokenType::LBRACE, lexeme, startLine, startColumn);
            case '}': return Token(TokenType::RBRACE, lexeme, startLine, startColumn);
            case '[': return Token(TokenType::LBRACKET, lexeme, startLine, startColumn);
            case ']': return Token(TokenType::RBRACKET, lexeme, startLine, startColumn);
            case ':': return Token(TokenType::COLON, lexeme, startLine, startColumn);
            case ',': return Token(TokenType::COMMA, lexeme, startLine, startColumn);
            case ';': return Token(TokenType::SEMICOLON, lexeme, startLine, startColumn);
            case '-': return Token(TokenType::HYPHEN, lexeme, startLine, startColumn);
            default:
                if (errorManager) {
                    errorManager->addError(lexeme, "Caracter ilegal", 
                        "El caracter '" + string(1, c) + "' no es válido en el lenguaje",
                        startLine, startColumn, ErrorSeverity::ERROR);
                }
                return Token(TokenType::ERROR, lexeme, startLine, startColumn);
        }
    }
}

Token LexicalAnalyzer::recognizeNumberOrDateTime() {
    int startLine = line;
    int startColumn = column;
    string lexeme;
    
    // Leer la parte numérica inicial
    while (!isEOF() && isdigit(currentChar)) {
        lexeme += currentChar;
        advance();
    }
    
    // Verificar si es una fecha (YYYY-MM-DD)
    if (currentChar == '-' && isdigit(peek())) {
        // Es una fecha, leer el resto
        lexeme += currentChar;
        advance();
        
        // Leer mes
        while (!isEOF() && isdigit(currentChar)) {
            lexeme += currentChar;
            advance();
        }
        
        if (currentChar == '-' && isdigit(peek())) {
            lexeme += currentChar;
            advance();
            
            // Leer día
            while (!isEOF() && isdigit(currentChar)) {
                lexeme += currentChar;
                advance();
            }
            
            // Validar fecha
            if (isValidDate(lexeme)) {
                return Token(TokenType::DATE, lexeme, startLine, startColumn);
            } else {
                if (errorManager) {
                    errorManager->addError(lexeme, "Fecha inválida", 
                        "La fecha '" + lexeme + "' no es válida. Formato esperado: AAAA-MM-DD",
                        startLine, startColumn, ErrorSeverity::ERROR);
                }
                return Token(TokenType::ERROR, lexeme, startLine, startColumn);
            }
        }
    }
    
    // Verificar si es una hora (HH:MM)
    if (currentChar == ':' && isdigit(peek())) {
        lexeme += currentChar;
        advance();
        
        // Leer minutos
        while (!isEOF() && isdigit(currentChar)) {
            lexeme += currentChar;
            advance();
        }
        
        // Validar hora
        if (isValidTime(lexeme)) {
            return Token(TokenType::TIME, lexeme, startLine, startColumn);
        } else {
            if (errorManager) {
                errorManager->addError(lexeme, "Hora inválida", 
                    "La hora '" + lexeme + "' no es válida. Formato esperado: HH:MM (00:00-23:59)",
                    startLine, startColumn, ErrorSeverity::ERROR);
            }
            return Token(TokenType::ERROR, lexeme, startLine, startColumn);
        }
    }
    
    // Es un número entero simple
    return Token(TokenType::INTEGER, lexeme, startLine, startColumn);
}

Token LexicalAnalyzer::recognizeIdentifier() {
    int startLine = line;
    int startColumn = column;
    string lexeme;
    
    // Leer el identificador completo
    while (!isEOF() && (isalnum(currentChar) || currentChar == '_')) {
        lexeme += currentChar;
        advance();
    }
    
    // Palabras reservadas del lenguaje
    if (lexeme == "HOSPITAL") return Token(TokenType::HOSPITAL, lexeme, startLine, startColumn);
    if (lexeme == "PACIENTES") return Token(TokenType::PACIENTES, lexeme, startLine, startColumn);
    if (lexeme == "MEDICOS") return Token(TokenType::MEDICOS, lexeme, startLine, startColumn);
    if (lexeme == "CITAS") return Token(TokenType::CITAS, lexeme, startLine, startColumn);
    if (lexeme == "DIAGNOSTICOS") return Token(TokenType::DIAGNOSTICOS, lexeme, startLine, startColumn);
    if (lexeme == "paciente") return Token(TokenType::PACIENTE, lexeme, startLine, startColumn);
    if (lexeme == "medico") return Token(TokenType::MEDICO, lexeme, startLine, startColumn);
    if (lexeme == "cita") return Token(TokenType::CITA, lexeme, startLine, startColumn);
    if (lexeme == "diagnostico") return Token(TokenType::DIAGNOSTICO, lexeme, startLine, startColumn);
    if (lexeme == "con") return Token(TokenType::CON, lexeme, startLine, startColumn);
    
    // Atributos comunes en el lenguaje médico
    if (lexeme == "edad" || lexeme == "tipo_sangre" || lexeme == "especialidad" ||
        lexeme == "codigo" || lexeme == "fecha" || lexeme == "hora" ||
        lexeme == "condicion" || lexeme == "medicamento" || lexeme == "dosis") {
        return Token(TokenType::ATRIBUTO, lexeme, startLine, startColumn);
    }
    
    // Verificar si es especialidad médica
    if (isValidSpecialty(lexeme)) {
        return Token(TokenType::ESPECIALIDAD, lexeme, startLine, startColumn);
    }
    
    // Verificar si es tipo de dosis
    if (isValidDosis(lexeme)) {
        return Token(TokenType::DOSIS, lexeme, startLine, startColumn);
    }
    
    // Verificar si es tipo de sangre
    if (isValidBloodType(lexeme)) {
        return Token(TokenType::BLOOD_TYPE, lexeme, startLine, startColumn);
    }
    
    // Error: identificador no reconocido
    if (errorManager) {
        errorManager->addError(lexeme, "Token inválido", 
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

    // Leer el número completo
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

    // Avanzar para saltar la comilla inicial
    advance();

    // Leer hasta encontrar la comilla de cierre
    while (!isEOF() && currentChar != '"')
    {
        lexeme += currentChar;
        advance();
    }

    // Verificar si se encontró la comilla de cierre
    if (currentChar == '"')
    {
        advance(); // Saltar la comilla de cierre
        return Token(TokenType::STRING, lexeme, startLine, startColumn);
    }
    else
    {
        // Cadena sin cerrar
        if (errorManager) {
            errorManager->addError(lexeme, "Cadena sin cerrar",
                                   "Se encontró el inicio de una cadena pero no se detectó el cierre",
                                   startLine, startColumn, ErrorSeverity::CRITICAL);
        }
        return Token(TokenType::ERROR, lexeme, startLine, startColumn);
    }
}

bool LexicalAnalyzer::isValidDate(const string &dateStr)
{
    // Implementar validación de fecha AAAA-MM-DD
    regex dateRegex(R"(\d{4}-\d{2}-\d{2})");
    if (!regex_match(dateStr, dateRegex))
        return false;

    // Extraer año, mes, día
    int year = stoi(dateStr.substr(0, 4));
    int month = stoi(dateStr.substr(5, 2));
    int day = stoi(dateStr.substr(8, 2));

    // Validar mes
    if (month < 1 || month > 12)
        return false;

    // Validar día según el mes
    int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (month == 2)
    {
        // Verificar año bisiesto
        bool isLeap = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
        if (isLeap)
            daysInMonth[1] = 29;
    }

    return day >= 1 && day <= daysInMonth[month - 1];
}

bool LexicalAnalyzer::isValidTime(const string &timeStr)
{
    regex timeRegex(R"(\d{2}:\d{2})");
    if (!regex_match(timeStr, timeRegex))
        return false;

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