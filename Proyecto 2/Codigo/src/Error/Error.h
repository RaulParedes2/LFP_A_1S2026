#ifndef ERROR_H
#define ERROR_H

#include <string>
using namespace std;

namespace TaskScript
{
    // Enumeracion de los tipos de errores posibles durante el analisis
    enum class ErrorType
    {
        LEXICO_ERROR,     // Error en el analisis Lexico
        SINTACTICO_ERROR, // Error en el analisis Sintactico
    };

    enum class ErrorSeverity
    {
        ERROR_NORMAL,     // Error comun que no detiene el analisis
        CRITICO_ERROR,    // Error grave que puede afectar el analisis posterior
        ADVERTENCIA       // Advertencia que no impide la ejecucion
    };

    class Error
    {
    private:
        int id;
        string lexeme;
        ErrorType type;
        string description;
        int line;
        int column;
        ErrorSeverity severity;

    public:
        Error(int id, const string& lexeme, ErrorType type, 
              const string& description, int line, int column, 
              ErrorSeverity severity = ErrorSeverity::ERROR_NORMAL);

        int getId() const;
        string getLexeme() const;
        ErrorType getType() const;
        string getDescription() const;
        string getTypeString() const;
        int getLine() const;
        int getColumn() const;
        ErrorSeverity getSeverity() const;
        string getSeverityString() const;
        string toHtmlRow() const;
    };
}
#endif // ERROR_H