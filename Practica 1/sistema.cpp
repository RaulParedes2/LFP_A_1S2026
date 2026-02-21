#include "sistema.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <exception>

using namespace std;

//----------------------------------------------------------------------------
// Carga de Estudiantes
//----------------------------------------------------------------------------

void Sistema::cargarEstudiantes(string ruta)
{

    ifstream archivo(ruta);
    try
    {
        if (!archivo.is_open())
        {
            throw runtime_error("Error al cargar el archivo: " + ruta);
            return;
        }
    }
    catch (const exception &e)
    {
        cout << "Error: " << e.what() << endl;
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
                    stoi(partes[0]),  // entero
                    partes[1],        // string
                    partes[2],        // string
                    partes[3],        // string
                    stoi(partes[4])); // entero
                estudiantes.push_back(e);
            }
            catch (...)
            {
                cout << "Error de Datos de Estudiante.\n";
            }
        }
    }

    archivo.close();
    cout << "Archivo Cerrado \n";
}
//----------------------------------------------------------------------------
// Carga de cursos
//----------------------------------------------------------------------------

void Sistema::cargarCursos(string ruta)
{
    ifstream archivo(ruta);

    try
    {
        if (!archivo.is_open())
        {
            throw runtime_error("Error al cargar el archivo Cursos" + ruta);
            return;
        }
    }
    catch (const exception &e)
    {
        cout << "Error: " << e.what() << endl;
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
                    stoi(partes[0]), // entero
                    partes[1],       // string
                    stoi(partes[2]), // entero
                    stoi(partes[3]), // entero
                    partes[4]        // string
                );
                cursos.push_back(c);
            }
            catch (...)
            {
                cout << "Error en los datos de cursos. \n";
            }
        }
    }
    archivo.close();
    cout << "Archivo Cerrado.\n";
}
//----------------------------------------------------------------------------
// Carga de Notas
//----------------------------------------------------------------------------

void Sistema::cargarNotas(string ruta)
{

    ifstream archivo(ruta);

    try
    {
        if (!archivo.is_open())
        {
            throw runtime_error("Error no se pudo abrir el archivo: " + ruta);
            return;
        }
    }
    catch (const exception &e)
    {
        cout << "Error" << e.what() << endl;
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
                    stoi(partes[0]),  // entero
                    stoi(partes[1]),  // entero
                    stod(partes[2]),  // double
                    partes[3],        // string
                    stoi(partes[4])); // entero

                notas.push_back(n);
            }
            catch (...)
            {
                cout << "Error en los datos.\n";
            }
        }
    }
    archivo.close();
    cout << " Archivo cerardo.\n";
}
