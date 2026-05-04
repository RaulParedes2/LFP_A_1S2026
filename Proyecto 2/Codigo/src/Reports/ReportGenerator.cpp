#include "ReportGenerator.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <map>
#include <functional>

namespace TaskScript {

ReportGenerator::ReportGenerator() {}

void ReportGenerator::loadData(const std::vector<Token>& tokensList,
                                const std::vector<Error>& errorsList) {
    this->tokens = tokensList;
    this->errors = errorsList;
}

void ReportGenerator::extractTasksFromAST(ASTNode* node, const std::string& currentColumn) {
    if (!node) return;
    
    // Buscar columna
    if (node->getName() == "columna") {
        std::string newColumn = currentColumn;
        for (const auto& child : node->getChildren()) {
            if (child->getName() == "CADENA") {
                newColumn = child->getToken().getLexeme();
                break;
            }
        }
        for (const auto& child : node->getChildren()) {
            extractTasksFromAST(child.get(), newColumn);
        }
    }
    // Buscar tarea
    else if (node->getName() == "tarea") {
    TaskInfo task;
    task.columna = currentColumn;
    task.prioridad = "MEDIA";
    task.responsable = "Sin asignar";
    task.fechaLimite = "No definida";

    for (const auto& child : node->getChildren()) {

        // Nombre de la tarea
        if (child->getName() == "CADENA" && task.nombre.empty()) {
            task.nombre = child->getToken().getLexeme();
        }

        // Buscar atributos
        if (child->getName() == "atributos") {

            for (const auto& atributo : child->getChildren()) {

                if (atributo->getName() == "atributo") {

                    auto& hijos = atributo->getChildren();

                    if (hijos.size() >= 2) {

                        std::string tipo = hijos[0]->getName();

                        // PRIORIDAD
                        if (tipo == "PRIORIDAD") {
                            auto& prioridadNodo = hijos[1]; // <prioridad>

                            if (!prioridadNodo->getChildren().empty()) {
                                auto& valor = prioridadNodo->getChildren()[0];
                                task.prioridad = valor->getName(); // ALTA/MEDIA/BAJA
                            }
                        }

                        // RESPONSABLE
                        else if (tipo == "RESPONSABLE") {
                            task.responsable = hijos[1]->getToken().getLexeme();
                        }

                        // FECHA
                        else if (tipo == "FECHA_LIMITE") {
                            task.fechaLimite = hijos[1]->getToken().getLexeme();
                        }
                    }
                }
            }
        }
    }

    tasks.push_back(task);

    std::cout << "DEBUG: Tarea: " << task.nombre 
              << " | Resp: " << task.responsable 
              << " | Prioridad: " << task.prioridad 
              << " | Fecha: " << task.fechaLimite << std::endl;
}
    else {
        for (const auto& child : node->getChildren()) {
            extractTasksFromAST(child.get(), currentColumn);
        }
    }
}

void ReportGenerator::loadAST(ASTNode* root) {
    tasks.clear();
    if (root) {
        std::cout << "DEBUG: Cargando AST" << std::endl;
        extractTasksFromAST(root, "");
        std::cout << "DEBUG: Total tareas: " << tasks.size() << std::endl;
    }
}

std::string ReportGenerator::generateKanbanReport() const {
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
    
    std::map<std::string, int> columnCount;
    for (const auto& task : tasks) {
        columnCount[task.columna]++;
    }
    html << "Total de columnas: " << columnCount.size() << "<br>\n";
    html << "Fecha de generacion: " << __DATE__ << " " << __TIME__ << "\n";
    html << "</div>\n";
    
    html << "<div class=\"kanban-container\">\n";
    
    std::map<std::string, std::vector<TaskInfo>> tasksByColumn;
    for (const auto& task : tasks) {
        tasksByColumn[task.columna].push_back(task);
    }
    
    for (const auto& [columnName, columnTasks] : tasksByColumn) {
        html << "<div class=\"kanban-column\">\n";
        html << "<h3>" << columnName << " (" << columnTasks.size() << " tareas)</h3>\n";
        
        for (const auto& task : columnTasks) {
            std::string priorityClass = "priority-" + task.prioridad;
            html << "<div class=\"task-card " << priorityClass << "\">\n";
            html << "<h4>" << task.nombre << "</h4>\n";
            
            std::string prioridadLower = task.prioridad;
            std::transform(prioridadLower.begin(), prioridadLower.end(), 
                          prioridadLower.begin(), ::tolower);
            html << "<span class=\"badge badge-" << prioridadLower << "\">" 
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

std::string ReportGenerator::generateResponsableReport() const {
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
    if (!stats.empty()) {
        html << std::fixed << std::setprecision(1) 
             << (double)totalTareas / stats.size();
    } else {
        html << "0";
    }
    html << "\n</div>\n";
    
    html << "<table border=\"1\">\n";
    html << "<thead>\n";
    html << "<tr><th>Responsable</th><th>Tareas Asignadas</th>"
         << "<th>Alta</th><th>Media</th><th>Baja</th>"
         << "<th>Distribucion</th></tr>\n";
    html << "</thead>\n<tbody>\n";
    
    for (const auto& stat : stats) {
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
        html << "</tr>\n";
    }
    
    html << "</tbody>\n<table>\n";
    
    html << "<div class=\"footer\">\n";
    html << "Reporte generado por TaskScript - Lenguajes Formales y Programacion\n";
    html << "</div>\n";
    html << "</body>\n</html>";
    
    return html.str();
}

std::string ReportGenerator::generateTokenReport() const {
    std::stringstream html;
    
    html << "<!DOCTYPE html>\n";
    html << "<html>\n<head>\n";
    html << "<meta charset=\"UTF-8\">\n";
    html << "<title>Tabla de Tokens - TaskScript</title>\n";
    html << getCommonCSS();
    html << "</head>\n<body>\n";
    
    html << "<h1>Tabla de Tokens</h1>\n";
    
    html << "<table border=\"1\">\n";
    html << "<thead>\n";
    html << "<tr><th>No.</th><th>Lexema</th><th>Tipo</th><th>Linea</th><th>Columna</th></tr>\n";
    html << "</thead>\n<tbody>\n";
    
    int count = 1;
    for (const auto& token : tokens) {
        if (token.getType() == TokenType::FIN_DE_ARCHIVO) continue;
        
        html << "<tr>\n";
        html << "<td>" << count++ << "</td>\n";
        html << "<td>" << token.getLexeme() << "</td>\n";
        html << "<td>" << token.typeToString() << "</td>\n";
        html << "<td>" << token.getLine() << "</td>\n";
        html << "<td>" << token.getColumn() << "</td>\n";
        html << "</tr>\n";
    }
    
    html << "</tbody>\n<table>\n";
    
    html << "<div class=\"footer\">\n";
    html << "Total de tokens validos: " << (count - 1) << "<br>\n";
    html << "Reporte generado por TaskScript - Lenguajes Formales y Programacion\n";
    html << "</div>\n";
    html << "</body>\n</html>";
    
    return html.str();
}

std::string ReportGenerator::getCommonCSS() const {
    return R"(
    <style>
        body { font-family: Arial, sans-serif; margin: 20px; background-color: #f5f5f5; }
        h1 { color: #333; border-bottom: 3px solid #4CAF50; padding-bottom: 10px; }
        .kanban-container { display: flex; gap: 20px; overflow-x: auto; padding: 20px 0; }
        .kanban-column { background-color: #e9e9e9; border-radius: 8px; min-width: 300px; padding: 10px; }
        .kanban-column h3 { background-color: #333; color: white; padding: 10px; border-radius: 5px; margin-top: 0; }
        .task-card { background-color: white; border-radius: 5px; margin: 10px 0; padding: 10px; box-shadow: 0 2px 4px rgba(0,0,0,0.1); }
        .priority-ALTA { border-left: 5px solid #ff4444; }
        .priority-MEDIA { border-left: 5px solid #ffbb33; }
        .priority-BAJA { border-left: 5px solid #00C851; }
        .badge { display: inline-block; padding: 3px 8px; border-radius: 3px; font-size: 12px; margin: 2px; }
        .badge-alta { background-color: #ff4444; color: white; }
        .badge-media { background-color: #ffbb33; color: #333; }
        .badge-baja { background-color: #00C851; color: white; }
        .progress-bar-container { background-color: #e0e0e0; border-radius: 10px; height: 20px; overflow: hidden; }
        .progress-bar { background-color: #4CAF50; height: 100%; border-radius: 10px; }
        .footer { margin-top: 40px; padding-top: 20px; border-top: 1px solid #ddd; text-align: center; font-size: 12px; }
        .stats-summary { background-color: #e8f5e9; padding: 15px; border-radius: 8px; margin: 20px 0; }
        table { border-collapse: collapse; width: 100%; margin: 20px 0; background-color: white; }
        th { background-color: #4CAF50; color: white; padding: 12px; text-align: left; }
        td { padding: 10px; border-bottom: 1px solid #ddd; }
    </style>
    )";
}

std::vector<ResponsableStats> ReportGenerator::calculateResponsableStats() const {
    std::map<std::string, ResponsableStats> statsMap;
    int totalTareas = tasks.size();
    
    for (const auto& task : tasks) {
        std::string responsable = task.responsable.empty() ? "Sin asignar" : task.responsable;
        
        if (statsMap.find(responsable) == statsMap.end()) {
            statsMap[responsable] = {responsable, 0, 0, 0, 0, 0.0};
        }
        
        statsMap[responsable].totalTareas++;
        
        if (task.prioridad == "ALTA") {
            statsMap[responsable].alta++;
        } else if (task.prioridad == "MEDIA") {
            statsMap[responsable].media++;
        } else if (task.prioridad == "BAJA") {
            statsMap[responsable].baja++;
        }
    }
    
    std::vector<ResponsableStats> stats;
    for (auto& pair : statsMap) {
        if (totalTareas > 0) {
            pair.second.porcentaje = (pair.second.totalTareas * 100.0) / totalTareas;
        }
        stats.push_back(pair.second);
    }
    
    std::sort(stats.begin(), stats.end(),
              [](const ResponsableStats& a, const ResponsableStats& b) {
                  return a.totalTareas > b.totalTareas;
              });
    
    return stats;
}

bool ReportGenerator::saveAllReports(const std::string& baseFilename) {
    bool success = true;
    
    std::string kanbanFile = baseFilename + "_kanban.html";
    std::string responsableFile = baseFilename + "_responsable.html";
    std::string tokenFile = baseFilename + "_tokens.html";
    
    std::ofstream kanban(kanbanFile);
    if (kanban.is_open()) {
        kanban << generateKanbanReport();
        kanban.close();
        std::cout << "Guardado: " << kanbanFile << std::endl;
    } else {
        std::cout << "Error guardando: " << kanbanFile << std::endl;
        success = false;
    }
    
    std::ofstream responsable(responsableFile);
    if (responsable.is_open()) {
        responsable << generateResponsableReport();
        responsable.close();
        std::cout << "Guardado: " << responsableFile << std::endl;
    } else {
        std::cout << "Error guardando: " << responsableFile << std::endl;
        success = false;
    }
    
    std::ofstream token(tokenFile);
    if (token.is_open()) {
        token << generateTokenReport();
        token.close();
        std::cout << "Guardado: " << tokenFile << std::endl;
    } else {
        std::cout << "Error guardando: " << tokenFile << std::endl;
        success = false;
    }
    
    return success;
}

void ReportGenerator::clear() {
    tasks.clear();
    tokens.clear();
    errors.clear();
}

} // namespace TaskScript