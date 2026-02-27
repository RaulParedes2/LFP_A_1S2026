#include "sistema.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <exception>
#include <algorithm>
#include <cmath>

using namespace std;

//----------------------------------------------------------------------------
// Carga de Estudiantes
//----------------------------------------------------------------------------

void Sistema::cargarEstudiantes(string nombreArchivo)
{

    ifstream archivo(rutaBase + nombreArchivo);
    ;

    if (!archivo.is_open())
    {
        cout << "Error: No se pudo abrir el archivo: "
             << rutaBase + nombreArchivo << endl;
        return; // Aquí sí es correcto usar return
    }

    string linea;

    while (getline(archivo, linea))
    {

        if (linea.empty())
            continue;

        stringstream ss(linea);
        string dato;
        vector<string> partes;

        while (getline(ss, dato, ','))
        {
            partes.push_back(dato);
        }

        if (partes.size() == 5)
        {
            try
            {
                Estudiante e(
                    stoi(partes[0]), // carnet
                    partes[1],       // nombre
                    partes[2],       // apellido
                    partes[3],       // carrera
                    stoi(partes[4])  // semestre
                );

                estudiantes.push_back(e);
            }
            catch (...)
            {
                cout << "Error en los datos del estudiante: "
                     << linea << endl;
            }
        }
    }

    archivo.close();
    cout << "Archivo Cerrado \n";
}
//----------------------------------------------------------------------------
// Carga de Cursos
//----------------------------------------------------------------------------

void Sistema::cargarCursos(string nombreArchivo)
{
    ifstream archivo(rutaBase + nombreArchivo);

    if (!archivo.is_open())
    {
        cout << "Error: No se pudo abrir el archivo de cursos: "
             << rutaBase + nombreArchivo << endl;
        return;
    }

    string linea;

    while (getline(archivo, linea))
    {
        if (linea.empty())
            continue;

        stringstream ss(linea);
        string dato;
        vector<string> partes;

        while (getline(ss, dato, ','))
        {
            partes.push_back(dato);
        }

        if (partes.size() == 5)
        {
            try
            {
                Curso c(
                    stoi(partes[0]),  // codigo
                    partes[1],        // nombre
                    stoi(partes[2]),  // creditos
                    stoi(partes[3]),  // semestre
                    partes[4]         // carrera
                );

                cursos.push_back(c);
            }
            catch (...)
            {
                cout << "Error en los datos del curso: "
                     << linea << endl;
            }
        }
    }

    archivo.close();
    cout << " Cursos cargados correctamente.\n";
}
//----------------------------------------------------------------------------
// Carga de Notas
//----------------------------------------------------------------------------

void Sistema::cargarNotas(string nombreArchivo)
{
    ifstream archivo(rutaBase + nombreArchivo);

    if (!archivo.is_open())
    {
        cout << "Error: No se pudo abrir el archivo de notas: "
             << rutaBase + nombreArchivo << endl;
        return;
    }

    string linea;

    while (getline(archivo, linea))
    {
        if (linea.empty())
            continue;

        stringstream ss(linea);
        string dato;
        vector<string> partes;

        while (getline(ss, dato, ','))
        {
            partes.push_back(dato);
        }

        if (partes.size() == 5)
        {
            try
            {
                Nota n(
                    stoi(partes[0]),  // carnet
                    stoi(partes[1]),  // codigo curso
                    stod(partes[2]),  // nota
                    partes[3],        // ciclo
                    stoi(partes[4])   // año
                );

                notas.push_back(n);
            }
            catch (...)
            {
                cout << "Error en los datos de nota: "
                     << linea << endl;
            }
        }
    }

    archivo.close();
    cout << "✔ Notas cargadas correctamente.\n";
}
//----------------------------------------------------------------------------
//Calculos Estadisticos
//----------------------------------------------------------------------------
// Promedio
//----------------------------------------------------------------------------

double Sistema :: calcularPromedio(vector<double> &datos){
    if(datos.empty()) 
    return 0;

    double suma = 0;

    for(double n : datos){
        suma +=n;
    }
    return suma/datos.size();
} 

//----------------------------------------------------------------------------
// Mediana
//----------------------------------------------------------------------------

double Sistema :: calcularMediana ( vector<double> datos){
    if (datos.empty()) return 0;

    sort(datos.begin(), datos.end());

    int n = datos.size();

    if (n % 2 == 0)
        return (datos[n/2 - 1] + datos[n/2]) / 2.0;
    else
        return datos[n/2];
}
//----------------------------------------------------------------------------
// DESVIACIÓN ESTÁNDAR
//----------------------------------------------------------------------------

double Sistema::calcularDesviacionEstandar(vector<double> &datos)
{
    if (datos.empty()) return 0;

    double promedio = calcularPromedio(datos);
    double suma = 0;

    for (double n : datos)
        suma += pow(n - promedio, 2);

    return sqrt(suma / datos.size());
}
//----------------------------------------------------------------------------
// Percentil
//----------------------------------------------------------------------------
double Sistema::calcularPercentil(vector<double> datos, double percentil)
{
    if (datos.empty()) return 0;

    sort(datos.begin(), datos.end());

    double pos = (percentil / 100.0) * (datos.size() - 1);
    int index = floor(pos);
    double fraccion = pos - index;

    if (index + 1 < datos.size())
        return datos[index] + fraccion * (datos[index + 1] - datos[index]);
    else
        return datos[index];
}
//----------------------------------------------------------------------------
// REPORTE 1 - Estadísticas Generales por Curso (HTML)
//----------------------------------------------------------------------------

void Sistema::reporteEstadisticasCursoHTML()
{
    ofstream html(rutaReportes+"reporte_estadisticas_curso.html");

    if (!html.is_open())
    {
        cout << "Error al crear el archivo HTML.\n";
        return;
    }

    html << "<!DOCTYPE html>\n";
    html << "<html>\n<head>\n";
    html << "<meta charset='UTF-8'>\n";
    html << "<title>Reporte Estadísticas por Curso</title>\n";
    html << "<style>\n";
    html << "body{font-family: Arial;}\n";
    html << "table{border-collapse: collapse; width: 100%;}\n";
    html << "th, td{border:1px solid black; padding:8px; text-align:center;}\n";
    html << "th{background-color:#4CAF50; color:white;}\n";
    html << "</style>\n";
    html << "</head>\n<body>\n";

    html << "<h1>Reporte: Estadísticas Generales por Curso</h1>\n";
    html << "<table>\n";
    html << "<tr>";
    html << "<th>Curso</th>";
    html << "<th>Estudiantes</th>";
    html << "<th>Promedio</th>";
    html << "<th>Máxima</th>";
    html << "<th>Mínima</th>";
    html << "<th>Desv. Estándar</th>";
    html << "<th>Mediana</th>";
    html << "</tr>\n";

    for (Curso &curso : cursos)
    {
        vector<double> notasCurso;

        for (Nota &nota : notas)
        {
            if (nota.getCodigoCurso() == curso.getCodigo())
            {
                notasCurso.push_back(nota.getNota());
            }
        }

        if (!notasCurso.empty())
        {
            double promedio = calcularPromedio(notasCurso);
            double mediana = calcularMediana(notasCurso);
            double desviacion = calcularDesviacionEstandar(notasCurso);

            double maxNota = *max_element(notasCurso.begin(), notasCurso.end());
            double minNota = *min_element(notasCurso.begin(), notasCurso.end());

            html << "<tr>";
            html << "<td>" << curso.getNombre() << "</td>";
            html << "<td>" << notasCurso.size() << "</td>";
            html << "<td>" << promedio << "</td>";
            html << "<td>" << maxNota << "</td>";
            html << "<td>" << minNota << "</td>";
            html << "<td>" << desviacion << "</td>";
            html << "<td>" << mediana << "</td>";
            html << "</tr>\n";
        }
    }

    html << "</table>\n";
    html << "</body>\n</html>";

    html.close();

    cout << " Reporte generado: reporte_estadisticas_curso.html\n";
}
