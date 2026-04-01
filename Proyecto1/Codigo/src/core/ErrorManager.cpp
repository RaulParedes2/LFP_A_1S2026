#include "../../include/core/ErrorManager.h"

ErrorManager::ErrorManager() : nextId(1) {}

void ErrorManager::addError(const std::string& lexeme, const std::string& errorType,
                           const std::string& description, int line, int column, 
                           ErrorSeverity severity) {
    errors.push_back(LexicalError(nextId++, lexeme, errorType, description, line, column, severity));
}

bool ErrorManager::hasErrors() const { 
    return !errors.empty(); 
}

const std::vector<LexicalError>& ErrorManager::getErrors() const { 
    return errors; 
}

void ErrorManager::clearErrors() { 
    errors.clear(); 
    nextId = 1; 
}

void ErrorManager::printErrors() const {
    if (errors.empty()) {
        std::cout << "No errors found." << std::endl;
        return;
    }
    
    std::cout << "\n=== LEXICAL ERRORS ===" << std::endl;
    for (const auto& error : errors) {
        std::cout << "Error #" << error.id << std::endl;
        std::cout << "  Lexeme: " << error.lexeme << std::endl;
        std::cout << "  Type: " << error.errorType << std::endl;
        std::cout << "  Description: " << error.description << std::endl;
        std::cout << "  Position: line " << error.line << ", column " << error.column << std::endl;
        std::cout << "  Severity: " << (error.severity == ErrorSeverity::ERROR ? "ERROR" : "CRITICAL") << std::endl;
        std::cout << std::endl;
    }
}

int ErrorManager::getErrorCount() const { 
    return errors.size(); 
}