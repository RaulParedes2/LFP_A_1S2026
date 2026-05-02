#include "Lexer/Token.h"
#include "Lexer/LexicalAnalyzer.h"
#include "Error/ErrorManager.h"
#include <iostream>
#include <iomanip>

using namespace TaskScript;

void printTokenTable(const std::vector<Token>& tokens) {
    std::cout << "\n=== TABLA DE TOKENS ===\n";
    std::cout << std::left 
              << std::setw(5) << "No."
              << std::setw(20) << "Tipo"
              << std::setw(25) << "Lexema"
              << std::setw(10) << "Linea"
              << std::setw(10) << "Columna"
              << std::endl;
    std::cout << std::string(70, '-') << std::endl;
    
    int count = 1;
    for (const auto& token : tokens) {
        if (token.getType() == TokenType::FIN_DE_ARCHIVO) continue;
        
        std::cout << std::left
                  << std::setw(5) << count++
                  << std::setw(20) << token.typeToString()
                  << std::setw(25) << token.getLexeme()
                  << std::setw(10) << token.getLine()
                  << std::setw(10) << token.getColumn()
                  << std::endl;
    }
    std::cout << std::endl;
}

void printAllTokens(LexicalAnalyzer& lexer) {
    Token token = lexer.getNextToken();
    
    while (token.getType() != TokenType::FIN_DE_ARCHIVO) {
        std::cout << token.toString() << std::endl;
        token = lexer.getNextToken();
    }
}

int main(int argc, char* argv[]) {
    std::cout << "=== ANALIZADOR LEXICO TASKSCRIPT ===\n";
    std::cout << "Lenguajes Formales y de Programacion\n";
    std::cout << "Proyecto 2 - AFD Manual\n\n";
    
    // Verificar que se proporciono un archivo
    if (argc < 2) {
        std::cout << "Uso: " << argv[0] << " <archivo.task>\n";
        std::cout << "Ejemplo: " << argv[0] << " ejemplo.task\n";
        return 1;
    }
    
    std::string filename = argv[1];
    std::cout << "Procesando archivo: " << filename << "\n";
    
    // Crear manejador de errores y analizador lexico
    ErrorManager errorManager;
    LexicalAnalyzer lexer(errorManager);
    
    // Cargar y analizar el archivo
    if (!lexer.loadFile(filename)) {
        std::cout << "Error: No se pudo cargar el archivo.\n";
        return 1;
    }
    
    // Mostrar todos los tokens
    printAllTokens(lexer);
    
    // Mostrar tabla de tokens
    printTokenTable(lexer.getTokens());
    
    // Mostrar errores si los hay
    if (errorManager.hasErrors()) {
        std::cout << "\n=== ERRORES DETECTADOS ===\n";
        std::cout << errorManager.getErrorStats() << "\n\n";
        std::cout << errorManager.generateHtmlErrorTable() << "\n";
    } else {
        std::cout << "\n=== ANALISIS EXITOSO ===\n";
        std::cout << "No se detectaron errores lexicos.\n";
        std::cout << lexer.getStatistics() << "\n";
    }
    
    return 0;
}