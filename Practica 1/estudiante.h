#ifndef Estudiante_H
#define Estudiante_H

#include <string>
#include <iostream>

using namespace std;

class Estudiante
{
    // atributo de la clase estudiante
private:
    int carnet;
    string nombre;
    string apellido;
    string carrera;
    int semestre;

public:
    // constructor de la clase
    Estudiante(int c, string n, string a, string car, int s)
    {

        carnet = c;
        nombre = n;
        apellido = a;
        carrera = car;
        semestre = s;
    }
    // devolucion del nombre
    string nombreCompleto()
    {
        return nombre + " " + apellido;
    }

    // getters
    int getCarnet() { return carnet; }
    string getNombre() { return nombre; }
    string getApellido() { return apellido; }
    string getCarrera() { return carrera; }
    int getSemestre() { return semestre; }
};

#endif