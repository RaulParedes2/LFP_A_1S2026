#include <iostream>
#include <exception>
#include <limits>
#include "sistema.h"

using namespace std;

int main()
{

    Sistema sistema;
    int opcion;
    string ruta;

    // Menu

    do
    {

        cout << "\n======================================\n";
        cout << "SISTEMA DE ANALISIS ACADEMICO \n";
        cout << "======================================\n";
        cout << "1. Cargar archivo del estudiante\n";
        cout << "2. Cargar archivo de cursos\n";
        cout << "3. cargar archivo de notas\n";
        cout << "4. Generar Reporte: Estadistica por curso\n";
        cout << "5. Generar Reporte: Redimiento por estudiante\n";
        cout << "6. Generar Reporte: Top 10 Mejores Estudiantes\n";
        cout << "7. Generar Reporte: Cursos con Mayor aprobacion\n";
        cout << "8. Generar Reporte: Analisis por Carrera\n";
        cout << "9. Salir\n";
        cout << "Seleccione una opcion: ";
        cin >> opcion;

        if (cin.fail())
        {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "Entrada invalida.\n";
            continue;
        }

        try
        {
            switch (opcion)
            {

            /*   case 1:
                cout << "Ingrese ruta del archivo estudiantes.lfp: ";
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                getline(cin, ruta);
                sistema.cargarEstudiantes(ruta);
                break;
                */
            case 1:
                sistema.cargarEstudiantes("estudiantes.lfp");
                break;

                /*case 2:
                    cout << "Ingrese ruta del archivo cursos.lfp: ";
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    getline(cin, ruta);
                    sistema.cargarCursos(ruta);
                    break;
                    */
            case 2:
                sistema.cargarCursos("cursos.lfp");
                break;

                /*case 3:
                    cout << "Ingrese ruta del archivo notas.lfp: ";
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    getline(cin, ruta);
                    sistema.cargarNotas(ruta);
                    break;
                    */
            case 3:
                sistema.cargarNotas("notas.lfp");
                break;

            case 4:
                sistema.reporteEstadisticasCursoHTML();
                break;

            case 5:
                sistema.reporteRendimientoEstudianteHTML();
                break;

            case 6:
                sistema.reporteTop10HTML();
                break;

            case 7:
                sistema.reporteIndiceReprobacionHTML();
                break;

            case 8:
                sistema.reporteAnalisisCarreraHTML();
                break;

            case 9:
                cout << "Saliendo del sistema...\n";
                break;

            default:
                cout << "Opcion invalida.\n";
                break;
            }
        }
        catch (const exception &e)
        {
            cout << "Error: " << e.what() << endl;
        }

    } while (opcion != 9);

    return 0;
}