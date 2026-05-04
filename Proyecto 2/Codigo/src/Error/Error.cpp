#include "Error.h"

using namespace std;

namespace TaskScript {
    Error::Error(int id, const string& lexeme, ErrorType type,
                 const string& description, int line, int column,
                 ErrorSeverity severity) 
                 : id(id), lexeme(lexeme), type(type), description(description),
                 line(line), column(column), severity(severity) {}
    
    int Error::getId() const { return id; }
    string Error::getLexeme() const { return lexeme; }
    
    ErrorType Error::getType() const { return type; }
    
    string Error::getTypeString() const {
        return (type == ErrorType::LEXICO_ERROR) ? "Lexico" : "Sintactico";
    }
    
    string Error::getDescription() const { return description; }
    int Error::getLine() const { return line; }
    int Error::getColumn() const { return column; }
    
    ErrorSeverity Error::getSeverity() const { return severity; }
    
    string Error::getSeverityString() const {
        if (severity == ErrorSeverity::ERROR_NORMAL) return "ERROR";
        if (severity == ErrorSeverity::CRITICO_ERROR) return "CRITICO";
        return "ADVERTENCIA";
    }

    string Error::toHtmlRow() const {
        string severityClass = (severity == ErrorSeverity::ERROR_NORMAL) ? "error" : "critico";
        return "<tr class=\"" + severityClass + "\">"
               "<td>" + to_string(id) + "</td>"
               "<td>" + lexeme + "</td>"
               "<td>" + getTypeString() + "</td>"
               "<td>" + description + "</td>"
               "<td>" + to_string(line) + "</td>"
               "<td>" + to_string(column) + "</td>"
               "<td>" + getSeverityString() + "</td>"
               "</table>";
    }
}