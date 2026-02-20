#include "sistema.h"
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

void Sistema:: cargarEstudiantes(string ruta){

    ifstream archivo (ruta);
    string linea;

    while(getline(archivo, linea)){
        stringstream ss(linea);
        string dato;
        vector<string>partes;

        while(getline(ss, dato,'s')){
            partes.push_back(dato);
        }
        if(partes.size() == 5){
            Estudiante e(
                stoi(partes[0]),
                partes[1],
                partes[2],
                partes[3],
                stoi(partes[4])
            );
            estudiantes.push_back(e);
        }
    }

    archivo.close();
    cout << "Estudiante cargados correctamente. \n";
}