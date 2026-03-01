#ifndef NOTA_H
#define NOTA_H

#include <string>
#include <iostream>

using namespace std;

class Nota
{
    // atributos de la clase nota
private:
    int carnet;
    int codigoCurso;
    double nota;
    string ciclo;
    int anio;

public:
    // constructor de la clase
    Nota(int c, int co, double n, string ci, int a)
    {

        carnet = c;
        codigoCurso = co;
        nota = n;
        ciclo = ci;
        anio = a;
    }
    // getters
    int getCarnet() { return carnet; }
    int getCodigoCurso() { return codigoCurso; }
    double getNota() { return nota; }
    string getCiclo() { return ciclo; }
    int getAnio() { return anio; }
};
#endif