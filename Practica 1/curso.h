#ifndef CURSO_H
#define Curso_H

#include <string>
#include <iostream>

using namespace std;

class Curso
{

    // Atributos de la clase curso
private:
    int codigo;
    string nombre;
    int creditos;
    int semestre;
    string carrera;

    // Constructor
public:
    Curso(int c, string n, int cre, int s, string car)
    {

        codigo = c;
        nombre = n;
        creditos = cre;
        semestre = s;
        carrera = car;
    }
    // Getters
    int getCodigo() { return codigo; }
    string getNombre() { return nombre; }
    int getCreditos() { return creditos; }
    int getSemestre() { return semestre; }
    string getCarrera() { return carrera; }
};
#endif