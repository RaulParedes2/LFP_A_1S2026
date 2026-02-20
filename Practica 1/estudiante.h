#ifndef Estudiante_H
#define Estudiante_H

#include <string>
#include <iostream>

using namespace std;

class Estudiante{
    private:
        int carnet;
        string nombre;
        string apellido;
        string carrera;
        int semestre;

    public:
    Estudiante(int c, string n, string a, string car, int s){

        carnet = c;
        nombre = n;
        apellido = a;
        carrera = car;
        semestre = s;

    }
    string nombreCompleto(){
        return nombre + " " + apellido;
    }
    
};

#endif