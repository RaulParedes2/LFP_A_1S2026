#ifndef LEXICAL_ANALYZER_H
#define LEXICAL_ANALYZER_H

#include "Token.h"
#include "../Error/ErrorManager.h"
#include <string>
#include <vector>
#include <fstream>

using namespace std;

namespace TaskScript{

    //Analaizador Lexico implementado con un automata finito determinista manual
    class LexicalAnalyzer{
        private:
            string sourceCode; //Codigo fuente completo del archivo .task
            size_t currentPos; //Posicion actual en el sourceCode
            int line; //Linea actual(empieza en 1)
            int column; //Columna actual(empieza en 1)
            vector<Token> tokens; //Lista de todos los tokens reconocidos
            ErrorManager& errors; //Referencia al manejador de errores para reportar errores léxicos
            string filename; //Nombre del archivo fuente para reportar errores con contexto

            //Metodos auxiliares para navegas por el codigo fuente
            char peek() const; //Mira el caracter actual sin avanzar la posicion
            char peekNext() const; //Mira el siguiente caracter sin avanzar la posicion
            char advance(); //Avanza al siguiente caracter
            bool isAtEnd() const; //Verifica si llego al final
            void skipWhitespace(); //Omitir espacios en blanco y comentarios
            void updatePosition(char c); //Actualiza la linea y columna segun el caracter leido

            //Metodos de reconocimiento para cada tipo de token(implementando AFD)
            Token readString(); // Reconoce cadenas entre comillas
            Token readNumberOrDate(); //Reconoce numeros o fechas (dependiendo del formato)
            Token readKeywordOrIdentifier(); //Reconoce palabras clave o identificadores
            Token readSymbol(); //Reconoce simbolos y operadores

            // Convierte string a TokenType para palabras reservadas
            TokenType getKeywordType(const string& word) const;

            public:
            LexicalAnalyzer(ErrorManager& errorManager); //Constructor que recibe una referencia al manejador de errores

            //carga un archivo .task para ser analizado
            bool loadFile(const string& filename);

            bool hasLexicalErrors() const;
            //Retorna el siguiente token del flujo (Metodo principal del AFD)
            Token getNextToken();

            //Retorna todos los tokens reconocidos hasta el momento
            const vector<Token>& getTokens() const;

            //Reinicia el analizador para un nuevo archivo
            void reset();

            //Retorna estadisticas del analisis
            string getStatistics() const;
    };

    
}

#endif // LEXICAL_ANALYZER_H