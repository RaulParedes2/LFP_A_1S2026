#ifndef ERROR_MANAGER_H
#define ERROR_MANAGER_H

#include "Error.h"
#include <vector>
#include <string>

using namespace std;

namespace TaskScript
{
    class ErrorManager
    {
    private:
        vector<Error> errors;
        int nextId;

    public:
        ErrorManager();
        void addError(const string &lexeme, ErrorType type, const string &description, 
                      int line, int column, ErrorSeverity severity = ErrorSeverity::ERROR_NORMAL);
        bool hasErrors() const;
        int getErrorCount() const;
        const vector<Error> &getAllErrors() const;
        void clear();
        string generateHtmlErrorTable() const;
        string getErrorStats() const;
        bool hasLexicalErrors() const;
        
    };
}
#endif // ERROR_MANAGER_H