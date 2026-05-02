#ifndef REPORT_GENERATOR_H
#define REPORT_GENERATOR_H

#include "../Lexer/Token.h"
#include "../Error/ErrorManager.h"
#include "../Parser/ASTNode.h"
#include <string>
#include <vector>
#include <map>

namespace TaskScript
{

    // Estructura para almacenar informacion de una tarea extraida del arbol
    struct TaskInfo
    {
        std::string nombre;
        std::string columna;
        std::string prioridad;
        std::string responsable;
        std::string fechaLimite;
    };

    // Estructura para estadisticas por responsable
    struct ResponsableStats
    {
        std::string nombre;
        int totalTareas;
        int alta;
        int media;
        int baja;
        double porcentaje;
    };

    // Generador de los tres reportes HTML requeridos
    class ReportGenerator
    {
    private:
        std::vector<TaskInfo> tasks;
        std::vector<Token> tokens;
        std::vector<Error> errors;

        // Extrae todas las tareas del arbol de derivacion
        void extractTasksFromAST(ASTNode *node, const std::string &currentColumn);

        // Calcula estadisticas por responsable
        std::vector<ResponsableStats> calculateResponsableStats() const;

        // Genera CSS embebido para los reportes
        std::string getCommonCSS() const;

    public:
        ReportGenerator();

        // Cargar datos para los reportes
        void loadData(const std::vector<Token> &tokensList,
                      const std::vector<Error> &errorsList);
        void loadAST(ASTNode *root);

        // Reporte 1: Tablero Kanban Visual
        std::string generateKanbanReport() const;

        // Reporte 2: Carga por Responsable
        std::string generateResponsableReport() const;

        // Reporte 3: Tabla de Tokens (adicional)
        std::string generateTokenReport() const;

        // Guardar todos los reportes a archivos HTML
        bool saveAllReports(const std::string &baseFilename);

        // Limpiar datos
        void clear();
    };

} // namespace TaskScript

#endif // REPORT_GENERATOR_H