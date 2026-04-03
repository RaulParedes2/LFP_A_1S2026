## Descripción

MedLexer es una herramienta de escritorio desarrollada en C++ que permite analizar, interpretar y reportar estructuras de datos médicos hospitalarios a partir de archivos de texto con extensión `.med`, estructurados bajo las reglas del mini-lenguaje **MedLang**.

El proyecto implementa un **Autómata Finito Determinista (AFD)** manualmente para el análisis léxico, sin utilizar herramientas automáticas de generación léxica ni expresiones regulares.

## Características

 Analizador léxico con AFD implementado manualmente
- Reconocimiento de tokens del lenguaje MedLang
- Tabla de tokens con posición exacta (línea y columna)
- Tabla de errores léxicos con recuperación de errores
- Interfaz gráfica en Qt 6
- Generación de 4 reportes HTML con CSS embebido
- Diagrama Graphviz de relaciones hospitalarias
- Detección de conflictos de horario en citas

## Dependencias

| Dependencia | Versión | Descripción |
|-------------|---------|-------------|
| **C++ Compiler** | C++17 o superior | MinGW 13.1.0 o superior |
| **Qt** | 6.11.0 | Framework para interfaz gráfica |
| **CMake** | 3.16 o superior | Sistema de construcción |
| **Graphviz** | Opcional | Para visualizar diagrama DOT |


## Verificar instalaciones

powershell
# Verificar compilador
g++ --version

# Verificar Qt
dir C:\Qt

# Verificar CMake
cmake --versión

Instalación y Compilación
1. Clonar el repositorio
2. Configurar entorno Qt (C:\Qt\6.11.0\mingw_64\)
3. Compilar el proyecto (.\build_clean.bat)
4. Ejecutar la aplicación (.\build\MedLexer.exe)

## Lenguaje MedLang

HOSPITAL {
    PACIENTES {
        paciente: "Nombre" [edad: 00, tipo_sangre: "Tipo"]
    }
    MEDICOS {
        medico: "Nombre" [especialidad: ESPECIALIDAD, codigo: "COD-000"]
    }
    CITAS {
        cita: "Paciente" con "Medico" [fecha: AAAA-MM-DD, hora: HH:MM]
    }
    DIAGNOSTICOS {
        diagnostico: "Paciente" [condicion: "Enfermedad", medicamento: "Medicina", dosis: DOSIS]
    }
}

## Uso de la Aplicación

# Interfaz Principal

- Cargar Archivo - Botón "Cargar Archivo .med"

- Analizar - Botón "Analizar" para procesar el código

- Ver Tokens - Pestaña "Tokens" muestra la tabla de tokens

- Ver Errores - Pestaña "Errores" muestra errores léxicos

- Generar Reportes - Botones de reportes 1-4

# Estructura del Proyecto

MedLexer/
├── include/
│   ├── core/
│   │   ├── Token.h
│   │   ├── ErrorManager.h
│   │   └── LexicalAnalyzer.h
│   ├── reports/
│   │   └── ReportGenerator.h
│   └── gui/
│       └── MainWindow.h
├── src/
│   ├── core/
│   │   ├── Token.cpp
│   │   ├── ErrorManager.cpp
│   │   └── LexicalAnalyzer.cpp
│   ├── reports/
│   │   └── ReportGenerator.cpp
│   └── gui/
│       └── MainWindow.cpp
├── examples/
│   ├── hospital_ejemplo1.med
│   └── hospital_ejemplo2.med
├── CMakeLists.txt
├── README.md
└── .gitignore

## Autores

- Daniel Predes - Desarrollo inicial - GitHub

