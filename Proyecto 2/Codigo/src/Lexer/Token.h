#ifndef TOKEN_H
#define TOKEN_H

#include <string>

using namespace std;

namespace TaskScript
{

    enum class TokenType
    {
        // Palabras reservadas del lenguaje TaskScript
        TABLERO,      // Palabra clave para definir el tablero principal
        COLUMNA,      // Palabra clave para definir una columna Kanban
        TAREA,        // Palabra clave para definir una tarea
        PRIORIDAD,    // Palabra clave para el atributo prioridad
        RESPONSABLE,  // Palabra clave para el atributo responsable
        FECHA_LIMITE, // Palabra clave para el atributo fecha limite

        // Enum de prioridad
        ALTA,
        MEDIA,
        BAJA,

        // Literales
        IDENTIFICADOR, // Para nombres de tablero/columna? La gramatica usa CADENA, pero creo que es mejor usar un identificador para eso
        CADENA,        // Texto entre comillas dobles
        FECHA,         // Fecha en formato YYYY-MM-DD
        ENTERO,        // Para números enteros

        // Delimitadores y operadores
        LLAVE_ABRE,
        LLAVE_CIERRA,
        CORCHETE_ABRE,
        CORCHETE_CIERRA,
        DOS_PUNTOS,
        COMA,
        PUNTO_Y_COMA,

        // Especiales
        // Token especial para indicar fin del archivo
        FIN_DE_ARCHIVO,
        // Token para caracteres no reconocidos (errores lexicos)
        DESCONOCIDO
    };

    class Token
    {
    public:
        Token();
        Token(TokenType type, const string& lexeme, int line, int column);
        // Getters (constantes correctos)
        TokenType getType() const;
        string getLexeme() const;
        int getLine() const;
        int getColumn() const;

        // Convierte el tipo de token a string para reportes y debugging
        string typeToString() const; // Método para convertir el tipo de token a string (para depuración Útil para debugging y reportes)
        string toString() const; // Método para representar el token como string (para depuración)
    private:
        TokenType type;     // Tipo del token (ej: CADENA, TABLERO, etc.)
        std::string lexeme; // Texto original que forma el token
        int line;           // Numero de linea donde aparece el token
        int column;         // Columna donde comienza el token
    };
}

#endif
