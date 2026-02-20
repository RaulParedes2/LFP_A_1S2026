#ifndef CURSO_H
#define Curso_H

#include <string>
#include <iostream>

using namespace std;

class Curso{

    private:

        int codigo;
        string nombre;
        int creditos;
        int semestre;
        string carrera;

        public:
        Curso(int c, string n, int cre, int s, string car){

            codigo = c;
            nombre = n;
            creditos = cre;
            semestre = s;
            carrera = car;

        }
};
#endif