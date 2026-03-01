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
    // atributos de la clase sistema

private:
    vector<Estudiante> estudiantes;
    vector<Curso> cursos;
    vector<Nota> notas;

    string rutaBase = "data/";         // nombre de la carpeta de los archivos lfp
    string rutaReportes = "reportes/"; // nombre de la carpeta de los reportes

    // Calculos estadisticos

    double calcularPromedio(vector<double> &datos);
    double calcularMediana(vector<double> datos);
    double calcularDesviacionEstandar(vector<double> &datos);
    double calcularPercentil(vector<double> datos, double percentil);

    // Duplicado de archivos

    bool estudiantesCargados = false;
    bool cursosCargados = false;
    bool notasCargadas = false;

public:
    void cargarEstudiantes(string ruta);
    void cargarCursos(string ruta);
    void cargarNotas(string ruta);

    void reporteEstadisticasCursoHTML();

    void reporteRendimientoEstudianteHTML();

    void reporteTop10HTML();

    void reporteIndiceReprobacionHTML();

    void reporteAnalisisCarreraHTML();
};
#endif