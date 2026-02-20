#ifndef SISTEMA_H
#define SISTEMA_H

#include <string>
#include <iostream>
#include <vector>
#include "estudiante.h"
#include "curso.h"
#include "nota.h"


class Sistema{

    private:
    vector<Estudiante>estudiantes;
    vector<Curso>cursos;
    vector<Nota>notas;

    public:
    
    void cargarEstudiantes(string ruta);
    void cargarCursos(string ruta);
    void cargarNotas(string ruta);

    void reporteEstadistica();
    void reporteRendimientoEstudiante();
    void reporteTop10();
    void reporteMayorReprobacion();
    void reporteAnalisisCarrera();


};
#endif