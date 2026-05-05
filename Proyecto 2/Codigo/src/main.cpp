#include "Lexer/Token.h"
#include "Lexer/LexicalAnalyzer.h"
#include "Error/ErrorManager.h"
#include "Parser/SyntaxAnalyzer.h"
#include "Reports/ReportGenerator.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <set>
#include <tuple>

using namespace TaskScript;

// Función para escapar cadenas JSON
std::string escapeJson(const std::string& str) {
    std::stringstream ss;
    for (char c : str) {
        switch (c) {
            case '"': ss << "\\\""; break;
            case '\\': ss << "\\\\"; break;
            case '\n': ss << "\\n"; break;
            case '\r': ss << "\\r"; break;
            case '\t': ss << "\\t"; break;
            default: ss << c; break;
        }
    }
    return ss.str();
}

// Generar archivo JSON con resultados
// Generar archivo JSON con resultados (sin duplicados)
void generateJsonReport(const std::vector<Token>& tokens, 
                        const ErrorManager& lexErrors,
                        const ErrorManager& syntaxErrors,
                        const std::string& filename,
                        const std::string& dotCode) {
    std::ofstream jsonFile(filename);
    if (!jsonFile.is_open()) return;
    
    jsonFile << "{\n";
    jsonFile << "  \"tokens\": [\n";
    
    int tokenCount = 1;
    for (size_t i = 0; i < tokens.size(); i++) {
        const auto& token = tokens[i];
        if (token.getType() == TokenType::FIN_DE_ARCHIVO) continue;
        
        jsonFile << "    {";
        jsonFile << "\"no\": " << tokenCount++ << ", ";
        jsonFile << "\"lexeme\": \"" << escapeJson(token.getLexeme()) << "\", ";
        jsonFile << "\"type\": \"" << token.typeToString() << "\", ";
        jsonFile << "\"line\": " << token.getLine() << ", ";
        jsonFile << "\"column\": " << token.getColumn();
        jsonFile << "}";
        if (i < tokens.size() - 1) jsonFile << ",";
        jsonFile << "\n";
    }
    
    jsonFile << "  ],\n";
    jsonFile << "  \"errors\": [\n";
    
    // Usar set para evitar duplicados
    std::set<std::tuple<std::string, int, int, std::string>> seen;
    std::vector<Error> uniqueErrors;
    
    // Agregar errores léxicos
    for (const auto& error : lexErrors.getAllErrors()) {
        auto key = std::make_tuple(error.getLexeme(), error.getLine(), error.getColumn(), error.getTypeString());
        if (seen.find(key) == seen.end()) {
            seen.insert(key);
            uniqueErrors.push_back(error);
        }
    }
    
    // Agregar errores sintácticos
    for (const auto& error : syntaxErrors.getAllErrors()) {
        auto key = std::make_tuple(error.getLexeme(), error.getLine(), error.getColumn(), error.getTypeString());
        if (seen.find(key) == seen.end()) {
            seen.insert(key);
            uniqueErrors.push_back(error);
        }
    }
    
    // Escribir errores únicos
    for (size_t i = 0; i < uniqueErrors.size(); i++) {
        const auto& error = uniqueErrors[i];
        jsonFile << "    {";
        jsonFile << "\"no\": " << (i + 1) << ", ";
        jsonFile << "\"lexeme\": \"" << escapeJson(error.getLexeme()) << "\", ";
        jsonFile << "\"type\": \"" << error.getTypeString() << "\", ";
        jsonFile << "\"description\": \"" << escapeJson(error.getDescription()) << "\", ";
        jsonFile << "\"line\": " << error.getLine() << ", ";
        jsonFile << "\"column\": " << error.getColumn() << ", ";
        jsonFile << "\"severity\": \"" << error.getSeverityString() << "\"";
        jsonFile << "}";
        if (i < uniqueErrors.size() - 1) jsonFile << ",";
        jsonFile << "\n";
    }
    
    jsonFile << "\n  ],\n";
    jsonFile << "  \"dotCode\": \"" << escapeJson(dotCode) << "\"\n";
    jsonFile << "}\n";
    
    jsonFile.close();
    std::cout << "JSON guardado: " << filename << std::endl;
}

void printTokenTable(const std::vector<Token>& tokens) {
    std::cout << "\n=== TABLA DE TOKENS ===\n";
    std::cout << std::left 
              << std::setw(5) << "No."
              << std::setw(20) << "Tipo"
              << std::setw(35) << "Lexema"
              << std::setw(10) << "Linea"
              << std::setw(10) << "Columna"
              << std::endl;
    std::cout << std::string(80, '-') << std::endl;
    
    int count = 1;
    for (const auto& token : tokens) {
        if (token.getType() == TokenType::FIN_DE_ARCHIVO) continue;
        
        std::string lexeme = token.getLexeme();
        if (lexeme.length() > 30) lexeme = lexeme.substr(0, 27) + "...";
        
        std::cout << std::left
                  << std::setw(5) << count++
                  << std::setw(20) << token.typeToString()
                  << std::setw(35) << lexeme
                  << std::setw(10) << token.getLine()
                  << std::setw(10) << token.getColumn()
                  << std::endl;
    }
    std::cout << std::endl;
}

void printErrorTable(const ErrorManager& errors) {
    if (errors.hasErrors()) {
        std::cout << "\n=== TABLA DE ERRORES ===\n";
        for (const auto& error : errors.getAllErrors()) {
            std::cout << "No. " << error.getId() 
                      << " | Lexema: " << error.getLexeme()
                      << " | Tipo: " << error.getTypeString()
                      << " | Linea: " << error.getLine()
                      << " | Columna: " << error.getColumn()
                      << " | " << error.getSeverityString()
                      << "\n     Descripcion: " << error.getDescription() << std::endl;
        }
        std::cout << std::endl;
    }
}

void saveDotFile(const std::string& filename, const std::string& dotCode) {
    std::ofstream file(filename);
    if (file.is_open()) {
        file << dotCode;
        file.close();
        std::cout << "Archivo DOT guardado: " << filename << std::endl;
    } else {
        std::cout << "Error al guardar el archivo DOT: " << filename << std::endl;
    }
}

int main(int argc, char* argv[]) {
    std::cout << "========================================\n";
    std::cout << "   ANALIZADOR TASKSCRIPT - PROYECTO 2   \n";
    std::cout << "========================================\n\n";
    
    if (argc < 2) {
        std::cout << "Uso: " << argv[0] << " <archivo.task> [nombre_reporte]\n";
        return 1;
    }
    
    std::string filename = argv[1];
    std::string reportBase = (argc >= 3) ? argv[2] : "reporte";
    
    std::cout << "Archivo de entrada: " << filename << "\n\n";
    
    // 1. Análisis léxico
    ErrorManager lexErrors;
    LexicalAnalyzer lexer(lexErrors);
    
    if (!lexer.loadFile(filename)) {
        std::cout << "Error fatal: No se pudo cargar el archivo.\n";
        return 1;
    }
    
    // Obtener todos los tokens
    std::vector<Token> tokens;
    Token token = lexer.getNextToken();
    while (token.getType() != TokenType::FIN_DE_ARCHIVO) {
        tokens.push_back(token);
        token = lexer.getNextToken();
    }
    
    std::cout << "Tokens encontrados: " << tokens.size() << "\n";
    printTokenTable(tokens);
    printErrorTable(lexErrors);
    std::cout << lexer.getStatistics() << "\n\n";
    
    // 2. Análisis sintáctico
    std::cout << "--- ANALISIS SINTACTICO ---\n";
    
    ErrorManager syntaxErrors;
    lexer.reset();
    SyntaxAnalyzer parser(lexer, syntaxErrors);
    
    auto ast = parser.parse();
    
    std::string dotCode = "";
    
    if (ast != nullptr) {
        std::cout << "Analisis sintactico exitoso!\n";
        std::cout << "Nodos del arbol: " << ast->getNodeCount() << "\n\n";
        ast->printTree();
        dotCode = parser.generateDotCode();
        saveDotFile(reportBase + "_arbol.dot", dotCode);
    } else {
        std::cout << "Analisis sintactico fallido.\n";
        printErrorTable(syntaxErrors);
    }
    
    // 3. Generar reportes HTML (SIEMPRE)
    std::cout << "\n--- GENERANDO REPORTES ---\n";
    
    ReportGenerator reporter;
    reporter.loadData(tokens, syntaxErrors.getAllErrors());
    
    if (ast != nullptr) {
        reporter.loadAST(ast.get());
    }
    
    if (reporter.saveAllReports("web/" +reportBase)) {
        std::cout << "Reportes generados:\n";
        std::cout << "  - " << reportBase << "_kanban.html\n";
        std::cout << "  - " << reportBase << "_responsable.html\n";
        std::cout << "  - " << reportBase << "_tokens.html\n";
    } else {
        std::cout << "Error al generar reportes.\n";
    }
    
    // 4. Generar JSON
    generateJsonReport(tokens, lexErrors, syntaxErrors, "resultados.json", dotCode);
    
    // 5. Resumen final
    std::cout << "\n=== RESUMEN FINAL ===\n";
    std::cout << "Errores lexicos: " << lexErrors.getErrorCount() << "\n";
    std::cout << "Errores sintacticos: " << syntaxErrors.getErrorCount() << "\n";
    std::cout << "Tokens reconocidos: " << tokens.size() << "\n";
    
    std::cout << "\nProceso completado.\n";
    
    return 0;
}