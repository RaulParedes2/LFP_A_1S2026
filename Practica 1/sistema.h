#ifndef SISTEMA_H
#define SISTEMA_H

#include <string>
#include <iostream>
#include <vector>
#include "estudiante.h"
#include "curso.h"
#include "nota.h"

using namespace std;

class Sistema
{

private:
    vector<Estudiante> estudiantes;
    vector<Curso> cursos;
    vector<Nota> notas;

    string rutaBase = "data/";
    string rutaReportes = "reportes/";

    double calcularPromedio(vector<double> &datos);
    double calcularMediana(vector<double> datos);
    double calcularDesviacionEstandar(vector<double> &datos);
    double calcularPercentil(vector<double> datos, double percentil);

    bool estudiantesCargados = false;
    bool cursosCargados = false;
    bool notasCargadas = false;

public:
    void cargarEstudiantes(string ruta);
    void cargarCursos(string ruta);
    void cargarNotas(string ruta);

    // void reporteEstadistica();
    void reporteEstadisticasCursoHTML();
    // void reporteRendimientoEstudiante();
    void reporteRendimientoEstudianteHTML();
    // void reporteTop10();
    void reporteTop10HTML();
    // void reporteMayorReprobacion();
    void reporteIndiceReprobacionHTML();
    // void reporteAnalisisCarrera();
    void reporteAnalisisCarreraHTML();
};
#endif