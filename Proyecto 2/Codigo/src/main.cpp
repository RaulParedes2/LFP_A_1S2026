#include "Lexer/Token.h"
#include "Lexer/LexicalAnalyzer.h"
#include "Error/ErrorManager.h"
#include "Parser/SyntaxAnalyzer.h"
#include "Reports/ReportGenerator.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>

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
void generateJsonReport(const std::vector<Token>& tokens, 
                        const ErrorManager& errors,
                        const std::string& filename,
                        const std::string& dotCode) {
    std::ofstream jsonFile(filename);
    if (!jsonFile.is_open()) return;
    
    jsonFile << "{\n";
    jsonFile << "  \"tokens\": [\n";
    
    for (size_t i = 0; i < tokens.size(); i++) {
        const auto& token = tokens[i];
        jsonFile << "    {";
        jsonFile << "\"no\": " << (i + 1) << ", ";
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
    
    const auto& errorList = errors.getAllErrors();
    for (size_t i = 0; i < errorList.size(); i++) {
        const auto& error = errorList[i];
        jsonFile << "    {";
        jsonFile << "\"no\": " << error.getId() << ", ";
        jsonFile << "\"lexeme\": \"" << escapeJson(error.getLexeme()) << "\", ";
        jsonFile << "\"type\": \"" << error.getTypeString() << "\", ";
        jsonFile << "\"description\": \"" << escapeJson(error.getDescription()) << "\", ";
        jsonFile << "\"line\": " << error.getLine() << ", ";
        jsonFile << "\"column\": " << error.getColumn() << ", ";
        jsonFile << "\"severity\": \"" << error.getSeverityString() << "\"";
        jsonFile << "}";
        if (i < errorList.size() - 1) jsonFile << ",";
        jsonFile << "\n";
    }
    
    jsonFile << "  ],\n";
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
        std::cout << std::left
                  << std::setw(5) << "No."
                  << std::setw(20) << "Lexema"
                  << std::setw(12) << "Tipo"
                  << std::setw(10) << "Linea"
                  << std::setw(10) << "Columna"
                  << std::setw(10) << "Gravedad"
                  << std::endl;
        std::cout << std::string(77, '-') << std::endl;
        
        for (const auto& error : errors.getAllErrors()) {
            std::string lexeme = error.getLexeme();
            if (lexeme.length() > 18) lexeme = lexeme.substr(0, 15) + "...";
            
            std::cout << std::left
                      << std::setw(5) << error.getId()
                      << std::setw(20) << lexeme
                      << std::setw(12) << error.getTypeString()
                      << std::setw(10) << error.getLine()
                      << std::setw(10) << error.getColumn()
                      << std::setw(10) << error.getSeverityString()
                      << std::endl;
            std::cout << "     Descripcion: " << error.getDescription() << std::endl;
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
        std::cout << "Ejemplo: " << argv[0] << " examples/Archivo_Correcto.task\n";
        return 1;
    }
    
    std::string filename = argv[1];
    std::string reportBase = (argc >= 3) ? argv[2] : "reporte";
    
    std::cout << "Archivo de entrada: " << filename << "\n\n";
    
    // 1. Inicializar componentes
    ErrorManager errorManager;
    LexicalAnalyzer lexer(errorManager);
    
    // 2. Cargar archivo
    if (!lexer.loadFile(filename)) {
        std::cout << "Error fatal: No se pudo cargar el archivo.\n";
        return 1;
    }
    
    // 3. Realizar analisis lexico completo
    std::cout << "--- ANALISIS LEXICO ---\n";
    Token token = lexer.getNextToken();
    while (token.getType() != TokenType::FIN_DE_ARCHIVO) {
        std::cout << token.toString() << std::endl;
        token = lexer.getNextToken();
    }
    
    // Mostrar tabla de tokens
    const auto& tokens = lexer.getTokens();
    printTokenTable(tokens);
    
    // Mostrar errores lexicos
    printErrorTable(errorManager);
    
    std::cout << lexer.getStatistics() << "\n\n";
    
    // 4. Realizar analisis sintactico
    std::cout << "--- ANALISIS SINTACTICO ---\n";
    
    // Crear nuevo manejador de errores para el parser
    ErrorManager syntaxErrors;
    
    // Reiniciar el lexer para empezar desde el principio
    lexer.reset();
    
    // Crear parser con el lexer reiniciado
    SyntaxAnalyzer parser(lexer, syntaxErrors);
    
    auto ast = parser.parse();
    
    // Verificar el resultado
    if (ast != nullptr) {
        std::cout << "Analisis sintactico completado exitosamente.\n";
        std::cout << "Arbol de derivacion generado con " << ast->getNodeCount() << " nodos.\n\n";
        
        // Imprimir arbol en consola
        std::cout << "=== ARBOL DE DERIVACION ===\n";
        ast->printTree();
        std::cout << std::endl;
        
        // 5. Generar reportes
        std::cout << "--- GENERANDO REPORTES ---\n";
        
        ReportGenerator reporter;
        reporter.loadData(tokens, syntaxErrors.getAllErrors());
        reporter.loadAST(ast.get());
        
        if (reporter.saveAllReports(reportBase)) {
            std::cout << "Reportes generados exitosamente:\n";
            std::cout << "  - " << reportBase << "_kanban.html\n";
            std::cout << "  - " << reportBase << "_responsable.html\n";
            std::cout << "  - " << reportBase << "_tokens.html\n";
        } else {
            std::cout << "Error al generar algunos reportes.\n";
        }
        
        // 6. Generar archivo DOT
        std::string dotCode = parser.generateDotCode();
        saveDotFile(reportBase + "_arbol.dot", dotCode);
        
        // 7. Generar archivo JSON para la interfaz web
        generateJsonReport(tokens, syntaxErrors, "resultados.json", dotCode);
        
        std::cout << "\nPara generar la imagen del arbol con Graphviz:\n";
        std::cout << "  dot -Tpng " << reportBase << "_arbol.dot -o " << reportBase << "_arbol.png\n";
        std::cout << "\nPara ver la interfaz web, abra web/index.html en su navegador.\n";
        
    } else {
        std::cout << "Analisis sintactico fallido debido a errores.\n";
        printErrorTable(syntaxErrors);
        
        // Generar JSON incluso si hay errores
        generateJsonReport(tokens, syntaxErrors, "resultados.json", "");
    }
    
    // 7. Mostrar resumen final
    std::cout << "\n=== RESUMEN FINAL ===\n";
    std::cout << "Errores lexicos: " << errorManager.getErrorCount() << "\n";
    std::cout << "Errores sintacticos: " << syntaxErrors.getErrorCount() << "\n";
    std::cout << "Tokens reconocidos: " << tokens.size() << "\n";
    
    std::cout << "\nProceso completado.\n";
    
    return 0;
}