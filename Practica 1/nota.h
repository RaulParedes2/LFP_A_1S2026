#ifndef NOTA_H
#define NOTA_H

#include <string>
#include <iostream>

using namespace std;

class Nota{
    private:

    int carnet;
    int codigoCurso;
    double nota;
    string ciclo;
    int anio;

    public:
    Nota(int c, int co, double n, string ci, int a){
        
        carnet = c;
        codigoCurso = co;
        nota = n;
        ciclo = ci;
        anio = a;
    }

};
#endif