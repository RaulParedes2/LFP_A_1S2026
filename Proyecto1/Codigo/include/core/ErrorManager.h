#ifndef ERROR_MANAGER_H
#define ERROR_MANAGER_H

#include <string>
#include <vector>
#include <iostream>

enum class ErrorSeverity {
    ERROR,
    CRITICAL
};

struct LexicalError {
    int id;
    std::string lexeme;
    std::string errorType;
    std::string description;
    int line;
    int column;
    ErrorSeverity severity;
    
    LexicalError(int id, const std::string& lexeme, const std::string& errorType,
                 const std::string& description, int line, int column, ErrorSeverity severity)
        : id(id), lexeme(lexeme), errorType(errorType), description(description),
          line(line), column(column), severity(severity) {}
};

class ErrorManager {
private:
    std::vector<LexicalError> errors;
    int nextId;
    
public:
    ErrorManager();
    
    void addError(const std::string& lexeme, const std::string& errorType,
                  const std::string& description, int line, int column, 
                  ErrorSeverity severity);
    
    bool hasErrors() const;
    const std::vector<LexicalError>& getErrors() const;
    void clearErrors();
    void printErrors() const;
    int getErrorCount() const;
};

#endif