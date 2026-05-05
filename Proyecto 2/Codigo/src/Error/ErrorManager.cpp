#include "ErrorManager.h"

using namespace std;

namespace TaskScript
{
    ErrorManager::ErrorManager() : nextId(1) {}

    void ErrorManager::addError(const string &lexeme, ErrorType type,
                                const string &description, int line, int column,
                                ErrorSeverity severity)
    {
        errors.emplace_back(nextId++, lexeme, type, description, line, column, severity);
    }

    bool ErrorManager::hasErrors() const { return !errors.empty(); }

    int ErrorManager::getErrorCount() const { return errors.size(); }
    const vector<Error> &ErrorManager::getAllErrors() const { return errors; }

    void ErrorManager::clear()
    {
        errors.clear();
        nextId = 1;
    }

    string ErrorManager::generateHtmlErrorTable() const
    {
        if (errors.empty())
            return "<p>No se encontraron errores.</p>";

        string html = "<h3>Tabla de Errores</h3>"
                      "<table border=\"1\" cellpadding=\"5\" cellspacing=\"0\">"
                      "<thead>"
                      "<tr style=\"background-color: #333; color: white;\">"
                      "<th>No.</th><th>Lexema/Token</th><th>Tipo</th>"
                      "<th>Descripcion</th><th>Linea</th><th>Columna</th><th>Gravedad</th>"
                      "</tr>"
                      "</thead><tbody>";
        for (const auto &error : errors)
            html += error.toHtmlRow();
        html += "</tbody></table>";
        html += "<p><strong>Total de errores:</strong> " + std::to_string(errors.size()) + "</p>";
        return html;
    }

    string ErrorManager::getErrorStats() const
    {
        int lexicoCount = 0, sintacticoCount = 0, criticoCount = 0;
        for (const auto &error : errors)
        {
            if (error.getType() == ErrorType::LEXICO_ERROR)
                lexicoCount++;
            else
                sintacticoCount++;
            if (error.getSeverity() == ErrorSeverity::CRITICO_ERROR)
                criticoCount++;
        }
        return "Errores Lexicos: " + std::to_string(lexicoCount) +
               " | Errores Sintacticos: " + std::to_string(sintacticoCount) +
               " | Errores Criticos: " + std::to_string(criticoCount);
    }
    bool ErrorManager::hasLexicalErrors() const
    {
        for (const auto &e : errors)
        {
            if (e.getType() == ErrorType::LEXICO_ERROR)
                return true;
        }
        return false;
    }

}