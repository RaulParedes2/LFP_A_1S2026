#include "ReportGenerator.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iomanip>

namespace TaskScript
{

    ReportGenerator::ReportGenerator() {}

    void ReportGenerator::loadData(const std::vector<Token> &tokensList,
                                   const std::vector<Error> &errorsList)
    {
        this->tokens = tokensList;
        this->errors = errorsList;
    }

    void ReportGenerator::extractTasksFromAST(ASTNode *node, const std::string &currentColumn)
    {
        if (!node)
            return;

        // Si encontramos un nodo de tipo "tarea", extraemos su informacion
        if (node->getName() == "tarea")
        {
            TaskInfo task;
            task.columna = currentColumn;

            // Recorrer los hijos para extraer atributos
            for (const auto &child : node->getChildren())
            {
                if (child->getName() == "CADENA" && task.nombre.empty())
                {
                    task.nombre = child->getToken().getLexeme();
                }
                else if (child->getName() == "atributos")
                {
                    // Recorrer atributos del hijo
                    for (const auto &attrNode : child->getChildren())
                    {
                        if (attrNode->getName() == "atributo")
                        {
                            for (const auto &attrChild : attrNode->getChildren())
                            {
                                if (attrChild->getName() == "PRIORIDAD")
                                {
                                    // Buscar el hijo de prioridad
                                    for (const auto &priorNode : attrChild->getChildren())
                                    {
                                        if (priorNode->getName() == "prioridad")
                                        {
                                            for (const auto &valNode : priorNode->getChildren())
                                            {
                                                if (!valNode->getIsTerminal())
                                                    continue;
                                                task.prioridad = valNode->getToken().typeToString();
                                            }
                                        }
                                    }
                                }
                                else if (attrChild->getName() == "RESPONSABLE")
                                {
                                    for (const auto &respNode : attrChild->getChildren())
                                    {
                                        if (respNode->getName() == "CADENA")
                                        {
                                            task.responsable = respNode->getToken().getLexeme();
                                        }
                                    }
                                }
                                else if (attrChild->getName() == "FECHA_LIMITE")
                                {
                                    for (const auto &fechaNode : attrChild->getChildren())
                                    {
                                        if (fechaNode->getName() == "FECHA")
                                        {
                                            task.fechaLimite = fechaNode->getToken().getLexeme();
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }

            if (!task.nombre.empty())
            {
                tasks.push_back(task);
            }
        }

        // Si encontramos una columna, actualizamos el contexto
        if (node->getName() == "columna")
        {
            std::string newColumn;
            for (const auto &child : node->getChildren())
            {
                if (child->getName() == "CADENA")
                {
                    newColumn = child->getToken().getLexeme();
                    break;
                }
            }

            // Recorrer las tareas dentro de esta columna
            for (const auto &child : node->getChildren())
            {
                if (child->getName() == "tareas")
                {
                    for (const auto &tareaNode : child->getChildren())
                    {
                        if (tareaNode->getName() == "tarea")
                        {
                            extractTasksFromAST(tareaNode.get(), newColumn);
                        }
                    }
                }
            }
        }

        // Recursivamente explorar todos los hijos
        for (const auto &child : node->getChildren())
        {
            extractTasksFromAST(child.get(), currentColumn);
        }
    }

    void ReportGenerator::loadAST(ASTNode *root)
    {
        tasks.clear();
        if (root)
        {
            extractTasksFromAST(root, "");
        }
    }

    std::vector<ResponsableStats> ReportGenerator::calculateResponsableStats() const
    {
        std::map<std::string, ResponsableStats> statsMap;
        int totalTareas = tasks.size();

        for (const auto &task : tasks)
        {
            std::string responsable = task.responsable.empty() ? "Sin asignar" : task.responsable;

            if (statsMap.find(responsable) == statsMap.end())
            {
                statsMap[responsable] = {responsable, 0, 0, 0, 0, 0.0};
            }

            statsMap[responsable].totalTareas++;

            if (task.prioridad == "ALTA")
            {
                statsMap[responsable].alta++;
            }
            else if (task.prioridad == "MEDIA")
            {
                statsMap[responsable].media++;
            }
            else if (task.prioridad == "BAJA")
            {
                statsMap[responsable].baja++;
            }
        }

        // Convertir map a vector y calcular porcentajes
        std::vector<ResponsableStats> stats;
        for (auto &pair : statsMap)
        {
            if (totalTareas > 0)
            {
                pair.second.porcentaje = (pair.second.totalTareas * 100.0) / totalTareas;
            }
            stats.push_back(pair.second);
        }

        // Ordenar por total de tareas descendente
        std::sort(stats.begin(), stats.end(),
                  [](const ResponsableStats &a, const ResponsableStats &b)
                  {
                      return a.totalTareas > b.totalTareas;
                  });

        return stats;
    }

    std::string ReportGenerator::getCommonCSS() const
    {
        return R"(
    <style>
        body {
            font-family: 'Segoe UI', Arial, sans-serif;
            margin: 20px;
            background-color: #f5f5f5;
        }
        h1 {
            color: #333;
            border-bottom: 3px solid #4CAF50;
            padding-bottom: 10px;
        }
        h2 {
            color: #555;
            margin-top: 30px;
        }
        h3 {
            color: #666;
        }
        table {
            border-collapse: collapse;
            width: 100%;
            margin: 20px 0;
            background-color: white;
            box-shadow: 0 1px 3px rgba(0,0,0,0.2);
        }
        th {
            background-color: #4CAF50;
            color: white;
            padding: 12px;
            text-align: left;
        }
        td {
            padding: 10px;
            border-bottom: 1px solid #ddd;
        }
        tr:hover {
            background-color: #f5f5f5;
        }
        .kanban-container {
            display: flex;
            gap: 20px;
            overflow-x: auto;
            padding: 20px 0;
        }
        .kanban-column {
            background-color: #e9e9e9;
            border-radius: 8px;
            min-width: 300px;
            padding: 10px;
            vertical-align: top;
        }
        .kanban-column h3 {
            background-color: #333;
            color: white;
            padding: 10px;
            border-radius: 5px;
            margin-top: 0;
        }
        .task-card {
            background-color: white;
            border-radius: 5px;
            margin: 10px 0;
            padding: 10px;
            box-shadow: 0 2px 4px rgba(0,0,0,0.1);
        }
        .task-card h4 {
            margin: 0 0 8px 0;
            color: #333;
        }
        .priority-ALTA {
            border-left: 5px solid #ff4444;
            background-color: #fff5f5;
        }
        .priority-MEDIA {
            border-left: 5px solid #ffbb33;
            background-color: #fffcf0;
        }
        .priority-BAJA {
            border-left: 5px solid #00C851;
            background-color: #f0fff4;
        }
        .badge {
            display: inline-block;
            padding: 3px 8px;
            border-radius: 3px;
            font-size: 12px;
            margin: 2px;
        }
        .badge-alta { background-color: #ff4444; color: white; }
        .badge-media { background-color: #ffbb33; color: #333; }
        .badge-baja { background-color: #00C851; color: white; }
        .progress-bar-container {
            background-color: #e0e0e0;
            border-radius: 10px;
            height: 20px;
            width: 100%;
            overflow: hidden;
        }
        .progress-bar {
            background-color: #4CAF50;
            height: 100%;
            border-radius: 10px;
            transition: width 0.3s;
        }
        .footer {
            margin-top: 40px;
            padding-top: 20px;
            border-top: 1px solid #ddd;
            text-align: center;
            font-size: 12px;
            color: #888;
        }
        .stats-summary {
            background-color: #e8f5e9;
            padding: 15px;
            border-radius: 8px;
            margin: 20px 0;
        }
    </style>
    )";
    }

    std::string ReportGenerator::generateKanbanReport() const
    {
        std::stringstream html;

        html << "<!DOCTYPE html>\n";
        html << "<html>\n<head>\n";
        html << "<meta charset=\"UTF-8\">\n";
        html << "<title>Reporte Kanban - TaskScript</title>\n";
        html << getCommonCSS();
        html << "</head>\n<body>\n";

        html << "<h1>Tablero Kanban - TaskScript</h1>\n";
        html << "<div class=\"stats-summary\">\n";
        html << "<strong>Resumen del Tablero:</strong><br>\n";
        html << "Total de tareas: " << tasks.size() << "<br>\n";

        // Contar columnas unicas
        std::map<std::string, int> columnCount;
        for (const auto &task : tasks)
        {
            columnCount[task.columna]++;
        }
        html << "Total de columnas: " << columnCount.size() << "<br>\n";
        html << "Fecha de generacion: " << __DATE__ << " " << __TIME__ << "\n";
        html << "</div>\n";

        html << "<div class=\"kanban-container\">\n";

        // Agrupar tareas por columna
        std::map<std::string, std::vector<TaskInfo>> tasksByColumn;
        for (const auto &task : tasks)
        {
            tasksByColumn[task.columna].push_back(task);
        }

        for (const auto &[columnName, columnTasks] : tasksByColumn)
        {
            html << "<div class=\"kanban-column\">\n";
            html << "<h3>" << columnName << " (" << columnTasks.size() << " tareas)</h3>\n";

            for (const auto &task : columnTasks)
            {
                std::string priorityClass = "priority-" + task.prioridad;
                html << "<div class=\"task-card " << priorityClass << "\">\n";
                html << "<h4>" << task.nombre << "</h4>\n";

                std::string prioridadLower = task.prioridad;
                std::transform(prioridadLower.begin(), prioridadLower.end(),
                               prioridadLower.begin(), [](unsigned char c)
                               { return std::tolower(c); });
                std::string badgeClass = "badge-" + prioridadLower;
                html << "<span class=\"badge " << badgeClass << "\">"
                     << task.prioridad << "</span><br>\n";
                html << "<strong>Responsable:</strong> " << task.responsable << "<br>\n";
                html << "<strong>Fecha limite:</strong> " << task.fechaLimite << "\n";
                html << "</div>\n";
            }
            html << "</div>\n";
        }

        html << "</div>\n";
        html << "<div class=\"footer\">\n";
        html << "Reporte generado por TaskScript - Lenguajes Formales y Programacion\n";
        html << "</div>\n";
        html << "</body>\n</html>";

        return html.str();
    }

    std::string ReportGenerator::generateResponsableReport() const
    {
        std::stringstream html;
        auto stats = calculateResponsableStats();
        int totalTareas = tasks.size();

        html << "<!DOCTYPE html>\n";
        html << "<html>\n<head>\n";
        html << "<meta charset=\"UTF-8\">\n";
        html << "<title>Reporte por Responsable - TaskScript</title>\n";
        html << getCommonCSS();
        html << R"(<style>
        .distribucion-cell { width: 200px; }
    </style>)";
        html << "</head>\n<body>\n";

        html << "<h1>Carga de Trabajo por Responsable</h1>\n";

        html << "<div class=\"stats-summary\">\n";
        html << "<strong>Resumen General:</strong><br>\n";
        html << "Total de tareas: " << totalTareas << "<br>\n";
        html << "Total de responsables: " << stats.size() << "<br>\n";
        html << "Promedio de tareas por responsable: ";
        if (!stats.empty())
        {
            html << std::fixed << std::setprecision(1)
                 << (double)totalTareas / stats.size();
        }
        else
        {
            html << "0";
        }
        html << "\n</div>\n";

        html << "<table>\n";
        html << "<thead>\n";
        html << "<tr><th>Responsable</th><th>Tareas Asignadas</th>"
             << "<th>Alta</th><th>Media</th><th>Baja</th>"
             << "<th>Distribucion</th></tr>\n";
        html << "</thead>\n<tbody>\n";

        for (const auto &stat : stats)
        {
            html << "<tr>\n";
            html << "<td>" << stat.nombre << "</td>\n";
            html << "<td><strong>" << stat.totalTareas << "</strong></td>\n";
            html << "<td>" << stat.alta << "</td>\n";
            html << "<td>" << stat.media << "</td>\n";
            html << "<td>" << stat.baja << "</td>\n";
            html << "<td class=\"distribucion-cell\">\n";
            html << "<div class=\"progress-bar-container\">\n";
            html << "<div class=\"progress-bar\" style=\"width: "
                 << stat.porcentaje << "%;\"></div>\n";
            html << "</div>\n";
            html << std::fixed << std::setprecision(1) << stat.porcentaje << "%\n";
            html << "</td>\n";
            html << "<tr>\n";
        }

        html << "</tbody>\n</table>\n";

        html << "<div class=\"footer\">\n";
        html << "Reporte generado por TaskScript - Lenguajes Formales y Programacion\n";
        html << "</div>\n";
        html << "</body>\n</html>";

        return html.str();
    }

    std::string ReportGenerator::generateTokenReport() const
    {
        std::stringstream html;

        html << "<!DOCTYPE html>\n";
        html << "<html>\n<head>\n";
        html << "<meta charset=\"UTF-8\">\n";
        html << "<title>Tabla de Tokens - TaskScript</title>\n";
        html << getCommonCSS();
        html << "</head>\n<body>\n";

        html << "<h1>Tabla de Tokens</h1>\n";

        html << "<table>\n";
        html << "<thead>\n";
        html << "<tr><th>No.</th><th>Lexema</th><th>Tipo</th><th>Linea</th><th>Columna</th></tr>\n";
        html << "</thead>\n<tbody>\n";

        int count = 1;
        for (const auto &token : tokens)
        {
            if (token.getType() == TokenType::FIN_DE_ARCHIVO)
                continue;

            html << "<tr>\n";
            html << "<td>" << count++ << "</td>\n";
            html << "<td>" << token.getLexeme() << "</td>\n";
            html << "<td>" << token.typeToString() << "</td>\n";
            html << "<td>" << token.getLine() << "</td>\n";
            html << "<td>" << token.getColumn() << "</td>\n";
            html << "</tr>\n";
        }

        html << "</tbody>\n</table>\n";

        html << "<div class=\"footer\">\n";
        html << "Total de tokens validos: " << (count - 1) << "<br>\n";
        html << "Reporte generado por TaskScript - Lenguajes Formales y Programacion\n";
        html << "</div>\n";
        html << "</body>\n</html>";

        return html.str();
    }

    bool ReportGenerator::saveAllReports(const std::string &baseFilename)
    {
        bool success = true;

        std::string kanbanFile = baseFilename + "_kanban.html";
        std::string responsableFile = baseFilename + "_responsable.html";
        std::string tokenFile = baseFilename + "_tokens.html";

        std::ofstream kanban(kanbanFile);
        if (kanban.is_open())
        {
            kanban << generateKanbanReport();
            kanban.close();
        }
        else
        {
            success = false;
        }

        std::ofstream responsable(responsableFile);
        if (responsable.is_open())
        {
            responsable << generateResponsableReport();
            responsable.close();
        }
        else
        {
            success = false;
        }

        std::ofstream token(tokenFile);
        if (token.is_open())
        {
            token << generateTokenReport();
            token.close();
        }
        else
        {
            success = false;
        }

        return success;
    }

    void ReportGenerator::clear()
    {
        tasks.clear();
        tokens.clear();
        errors.clear();
    }

} // namespace TaskScript