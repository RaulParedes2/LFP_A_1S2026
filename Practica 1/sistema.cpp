#include "sistema.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <exception>
#include <algorithm>
#include <map>
#include <cmath>

using namespace std;

//----------------------------------------------------------------------------
// Carga de Estudiantes
//----------------------------------------------------------------------------

void Sistema::cargarEstudiantes(string nombreArchivo)
{
     if (estudiantesCargados)
    {
        cout << "Recargando estudiantes...\n";
        estudiantes.clear();
    }

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
    estudiantesCargados = true;
    archivo.close();

    cout << "Estudiantes cargados correctamente.\n";
}
//----------------------------------------------------------------------------
// Carga de Cursos
//----------------------------------------------------------------------------

void Sistema::cargarCursos(string nombreArchivo)
{

     if (cursosCargados)
    {
        cout << "Recargando Cursos...\n";
        cursos.clear();
    }

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
    cursosCargados = true;
    archivo.close();
    cout << " Cursos cargados correctamente.\n";
}
//----------------------------------------------------------------------------
// Carga de Notas
//----------------------------------------------------------------------------

void Sistema::cargarNotas(string nombreArchivo)
{
     if (notasCargadas)
    {
        cout << "Recargando notas...\n";
        notas.clear();
    }

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

    notasCargadas = true;
    archivo.close();
    cout << " Notas cargadas correctamente.\n";
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

//----------------------------------------------------------------------------
// REPORTE 2 - Rendimiento por Estudiante (HTML)
//----------------------------------------------------------------------------

void Sistema::reporteRendimientoEstudianteHTML()
{
    ofstream html(rutaReportes + "reporte_rendimiento_estudiante.html");

    if (!html.is_open())
    {
        cout << "Error al crear el archivo HTML.\n";
        return;
    }

    html << "<!DOCTYPE html>\n";
    html << "<html>\n<head>\n";
    html << "<meta charset='UTF-8'>\n";
    html << "<title>Reporte Rendimiento por Estudiante</title>\n";
    html << "<style>\n";
    html << "body{font-family: Arial;}\n";
    html << "table{border-collapse: collapse; width: 100%;}\n";
    html << "th, td{border:1px solid black; padding:8px; text-align:center;}\n";
    html << "th{background-color:#2196F3; color:white;}\n";
    html << "</style>\n";
    html << "</head>\n<body>\n";

    html << "<h1>Reporte: Rendimiento por Estudiante</h1>\n";
    html << "<table>\n";
    html << "<tr>";
    html << "<th>Nombre</th>";
    html << "<th>Carnet</th>";
    html << "<th>Carrera</th>";
    html << "<th>Semestre</th>";
    html << "<th>Promedio General</th>";
    html << "<th>Aprobados</th>";
    html << "<th>Reprobados</th>";
    html << "<th>Créditos Acumulados</th>";
    html << "</tr>\n";

    for (Estudiante &est : estudiantes)
    {
        vector<double> notasEstudiante;
        int aprobados = 0;
        int reprobados = 0;
        int creditosAcumulados = 0;

        for (Nota &nota : notas)
        {
            if (nota.getCarnet() == est.getCarnet())
            {
                notasEstudiante.push_back(nota.getNota());

                if (nota.getNota() >= 61)
                {
                    aprobados++;

                    // Buscar curso para sumar créditos
                    for (Curso &curso : cursos)
                    {
                        if (curso.getCodigo() == nota.getCodigoCurso())
                        {
                            creditosAcumulados += curso.getCreditos();
                            break;
                        }
                    }
                }
                else
                {
                    reprobados++;
                }
            }
        }

        double promedio = calcularPromedio(notasEstudiante);

        html << "<tr>";
        html << "<td>" << est.nombreCompleto() << "</td>";
        html << "<td>" << est.getCarnet() << "</td>";
        html << "<td>" << est.getCarrera() << "</td>";
        html << "<td>" << est.getSemestre() << "</td>";
        html << "<td>" << promedio << "</td>";
        html << "<td>" << aprobados << "</td>";
        html << "<td>" << reprobados << "</td>";
        html << "<td>" << creditosAcumulados << "</td>";
        html << "</tr>\n";
    }

    html << "</table>\n";
    html << "</body>\n</html>";

    html.close();

    cout << "Reporte generado: reporte_rendimiento_estudiante.html\n";
}

//----------------------------------------------------------------------------
// REPORTE 3 - Top 10 Mejores Estudiantes (HTML)
//----------------------------------------------------------------------------

void Sistema::reporteTop10HTML()
{
    ofstream html(rutaReportes + "reporte_top10.html");

    if (!html.is_open())
    {
        cout << "Error al crear el archivo HTML.\n";
        return;
    }

    // Estructura auxiliar
    struct InfoEstudiante
    {
        Estudiante estudiante;
        double promedio;
    };

    vector<InfoEstudiante> lista;

    // Calcular promedio por estudiante
    for (Estudiante &est : estudiantes)
    {
        vector<double> notasEstudiante;

        for (Nota &nota : notas)
        {
            if (nota.getCarnet() == est.getCarnet())
            {
                notasEstudiante.push_back(nota.getNota());
            }
        }

        double promedio = calcularPromedio(notasEstudiante);

        lista.push_back({est, promedio});
    }

    // Ordenar de mayor a menor promedio
    sort(lista.begin(), lista.end(),
         [](InfoEstudiante &a, InfoEstudiante &b)
         {
             return a.promedio > b.promedio;
         });

    // HTML
    html << "<!DOCTYPE html>\n";
    html << "<html>\n<head>\n";
    html << "<meta charset='UTF-8'>\n";
    html << "<title>Top 10 Mejores Estudiantes</title>\n";
    html << "<style>\n";
    html << "body{font-family: Arial;}\n";
    html << "table{border-collapse: collapse; width: 100%;}\n";
    html << "th, td{border:1px solid black; padding:8px; text-align:center;}\n";
    html << "th{background-color:#FF9800; color:white;}\n";
    html << "</style>\n";
    html << "</head>\n<body>\n";

    html << "<h1>Top 10 Mejores Estudiantes</h1>\n";
    html << "<table>\n";
    html << "<tr>";
    html << "<th>Posición</th>";
    html << "<th>Carnet</th>";
    html << "<th>Nombre</th>";
    html << "<th>Carrera</th>";
    html << "<th>Semestre</th>";
    html << "<th>Promedio</th>";
    html << "</tr>\n";

    int limite = min(10, (int)lista.size());

    for (int i = 0; i < limite; i++)
    {
        html << "<tr>";
        html << "<td>" << i + 1 << "</td>";
        html << "<td>" << lista[i].estudiante.getCarnet() << "</td>";
        html << "<td>" << lista[i].estudiante.nombreCompleto() << "</td>";
        html << "<td>" << lista[i].estudiante.getCarrera() << "</td>";
        html << "<td>" << lista[i].estudiante.getSemestre() << "</td>";
        html << "<td>" << lista[i].promedio << "</td>";
        html << "</tr>\n";
    }

    html << "</table>\n";
    html << "</body>\n</html>";

    html.close();

    cout << " Reporte generado: reporte_top10.html\n";
}

//----------------------------------------------------------------------------
// REPORTE 4 - Cursos con Mayor Índice de Reprobación (HTML)
//----------------------------------------------------------------------------

void Sistema::reporteIndiceReprobacionHTML()
{
    ofstream html(rutaReportes + "reporte_indice_reprobacion.html");

    if (!html.is_open())
    {
        cout << "Error al crear el archivo HTML.\n";
        return;
    }

    struct InfoCurso
    {
        Curso curso;
        int total;
        int aprobados;
        int reprobados;
        double porcentaje;
    };

    vector<InfoCurso> lista;

    for (Curso &curso : cursos)
    {
        int total = 0;
        int aprobados = 0;
        int reprobados = 0;

        for (Nota &nota : notas)
        {
            if (nota.getCodigoCurso() == curso.getCodigo())
            {
                total++;

                if (nota.getNota() >= 61)
                    aprobados++;
                else
                    reprobados++;
            }
        }

        if (total > 0)
        {
            double porcentaje = (double)reprobados * 100.0 / total;

            lista.push_back({curso, total, aprobados, reprobados, porcentaje});
        }
    }

    // Ordenar de mayor a menor porcentaje de reprobación
    sort(lista.begin(), lista.end(),
         [](InfoCurso &a, InfoCurso &b)
         {
             return a.porcentaje > b.porcentaje;
         });

    // HTML
    html << "<!DOCTYPE html>\n";
    html << "<html>\n<head>\n";
    html << "<meta charset='UTF-8'>\n";
    html << "<title>Índice de Reprobación</title>\n";
    html << "<style>\n";
    html << "body{font-family: Arial;}\n";
    html << "table{border-collapse: collapse; width: 100%;}\n";
    html << "th, td{border:1px solid black; padding:8px; text-align:center;}\n";
    html << "th{background-color:#F44336; color:white;}\n";
    html << "</style>\n";
    html << "</head>\n<body>\n";

    html << "<h1>Cursos con Mayor Índice de Reprobación</h1>\n";
    html << "<table>\n";
    html << "<tr>";
    html << "<th>Código</th>";
    html << "<th>Curso</th>";
    html << "<th>Total</th>";
    html << "<th>Aprobados</th>";
    html << "<th>Reprobados</th>";
    html << "<th>% Reprobación</th>";
    html << "</tr>\n";

    for (InfoCurso &info : lista)
    {
        html << "<tr>";
        html << "<td>" << info.curso.getCodigo() << "</td>";
        html << "<td>" << info.curso.getNombre() << "</td>";
        html << "<td>" << info.total << "</td>";
        html << "<td>" << info.aprobados << "</td>";
        html << "<td>" << info.reprobados << "</td>";
        html << "<td>" << info.porcentaje << "%</td>";
        html << "</tr>\n";
    }

    html << "</table>\n";
    html << "</body>\n</html>";

    html.close();

    cout << " Reporte generado: reporte_indice_reprobacion.html\n";
}
//----------------------------------------------------------------------------
// REPORTE 5 - Análisis por Carrera (HTML)
//----------------------------------------------------------------------------

void Sistema::reporteAnalisisCarreraHTML()
{
    ofstream html(rutaReportes + "reporte_analisis_carrera.html");

    if (!html.is_open())
    {
        cout << "Error al crear el archivo HTML.\n";
        return;
    }

    // Agrupar estudiantes por carrera
    map<string, vector<Estudiante>> mapaCarreras;

    for (Estudiante &est : estudiantes)
    {
        mapaCarreras[est.getCarrera()].push_back(est);
    }

    // HTML base
    html << "<!DOCTYPE html>\n";
    html << "<html>\n<head>\n";
    html << "<meta charset='UTF-8'>\n";
    html << "<title>Análisis por Carrera</title>\n";
    html << "<style>\n";
    html << "body{font-family: Arial;}\n";
    html << "table{border-collapse: collapse; width: 100%; margin-bottom:40px;}\n";
    html << "th, td{border:1px solid black; padding:8px; text-align:center;}\n";
    html << "th{background-color:#673AB7; color:white;}\n";
    html << "</style>\n";
    html << "</head>\n<body>\n";

    html << "<h1>Análisis por Carrera</h1>\n";

    for (auto &par : mapaCarreras)
    {
        string nombreCarrera = par.first;
        vector<Estudiante> listaEst = par.second;

        int totalEstudiantes = listaEst.size();
        vector<double> notasCarrera;

        // Distribución por semestre
        map<int, int> distribucionSemestres;

        for (Estudiante &est : listaEst)
        {
            distribucionSemestres[est.getSemestre()]++;

            for (Nota &nota : notas)
            {
                if (nota.getCarnet() == est.getCarnet())
                {
                    notasCarrera.push_back(nota.getNota());
                }
            }
        }

        double promedioCarrera = calcularPromedio(notasCarrera);

        // Contar cursos disponibles para la carrera
        int cursosDisponibles = 0;
        for (Curso &curso : cursos)
        {
            if (curso.getCarrera() == nombreCarrera)
                cursosDisponibles++;
        }

        // Sección HTML por carrera
        html << "<h2>Carrera: " << nombreCarrera << "</h2>\n";
        html << "<table>\n";
        html << "<tr><th>Total Estudiantes</th><td>" << totalEstudiantes << "</td></tr>";
        html << "<tr><th>Promedio General</th><td>" << promedioCarrera << "</td></tr>";
        html << "<tr><th>Cursos Disponibles</th><td>" << cursosDisponibles << "</td></tr>";
        html << "</table>\n";

        // Tabla distribución semestres
        html << "<h3>Distribución por Semestre</h3>\n";
        html << "<table>\n";
        html << "<tr><th>Semestre</th><th>Cantidad Estudiantes</th></tr>\n";

        for (auto &sem : distribucionSemestres)
        {
            html << "<tr>";
            html << "<td>" << sem.first << "</td>";
            html << "<td>" << sem.second << "</td>";
            html << "</tr>\n";
        }

        html << "</table>\n";
    }

    html << "</body>\n</html>";

    html.close();

    cout << " Reporte generado: reporte_analisis_carrera.html\n";
}