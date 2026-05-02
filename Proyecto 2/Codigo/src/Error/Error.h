#ifndef ERROR_H
#define ERROR_H

#include <string>
using namespace std;

namespace TaskScript
{

    // Enumeracion de los tipos de errores posibles durante el analisis
    enum class ErrorType
    {
        LEXICO,     // Error en el analisis Lexio(Caracter no reconocido, cadena sin cerrar, etc)
        SINTACTICO, // Error en el analisis Sintactico (estructura gramatical incorrecta, etc)
        SEMANTICO   // Error en el analisis Semantico (uso incorrecto de los elementos del lenguaje, etc)
    };

    enum class ErrorSeverity
    {
        ERROR,      // Error comun que no detiene el analisis
        CRITICO,    // Error grave que puede afectar el analisis posterior
        ADVERTENCIA // Advertencia que no impide la ejecucion del programa

    };

    // Clase que representa un error individual detectado durante el analisis
    class Error
    {
    private:
        int id;                 // Identificacor unico incremental
        string lexeme;          // Lexema o token que causo el error
        ErrorType type;         // Tipo de error (Lexico o Sintactico)
        string description;     // Descripcion detallada del error
        int line;               // Linea donde se encontro el error
        int column;             // Columna donde se encontro el error
        ErrorSeverity severity; // Gravedad del error (Error, Critico, Advertencia)

    public:

    Error(int id, const string& lexeme, ErrorType type, 
          const string& description, int line, int column, 
          ErrorSeverity severity = ErrorSeverity::ERROR);

    // Getters para acceder a los atributos del error
    int getId() const;
    string getLexeme() const;
    ErrorType getType() const;
    string getDescription() const;
    string getTypeString() const; // Método para convertir el tipo de error a string (para depuración)
    int getLine() const;
    int getColumn() const;
    ErrorSeverity getSeverity() const;
    string getSeverityString() const; // Método para convertir la gravedad del error a string (para depuración)

    string toHtmlRow() const; // Método para representar el error como una fila de tabla HTML (para reportes)

    };

}
#endif // ERROR_H