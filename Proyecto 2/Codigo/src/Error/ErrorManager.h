#ifndef ERROR_MANAGER_H
#define ERROR_MANAGER_H

#include "Error.h"
#include <vector>
#include <string>

using namespace std;

namespace TaskScript
{

    // Clase que gestiona la colección de errores detectados durante el análisis
    class ErrorManager
    {
    private:
        vector<Error> errors; // Vector para almacenar los errores detectados
        int nextId;           // Contador para asignar IDs únicos a cada error

    public:
        ErrorManager(); // Constructor para inicializar el contador de IDs

        void addError(const string &lexeme, ErrorType type, const string &description, int line, int column, ErrorSeverity severity = ErrorSeverity::ERROR);
        // Verifica si hay errores registrados
        bool hasErrors() const;
        // Retorna el número total de errores registrados
        int getErrorCount() const;

        const vector<Error> &getAllErrors() const; // Método para obtener la lista de errores detectados (útil para pruebas y depuración)
        void clear();                              // Método para limpiar la lista de errores (útil para reiniciar el análisis)

        // Genera tabla HTML de errores para reportes
        string generateHtmlErrorTable() const;

        //retorn estadisticas de errores para el reporte final
        string getErrorStats() const;
    };


}

#endif // ERROR_MANAGER_H