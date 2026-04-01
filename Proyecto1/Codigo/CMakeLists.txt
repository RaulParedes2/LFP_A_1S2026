cmake_minimum_required(VERSION 3.16)
project(MedLexer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

# Buscar Qt6 (ajustar según tu versión)
# Si tienes Qt5, cambia a find_package(Qt5 REQUIRED COMPONENTS Core Widgets)
find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

# Incluir directorios
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${Qt6_INCLUDE_DIRS})

# Archivos fuente
set(SOURCES
    src/main_gui.cpp
    src/gui/MainWindow.cpp
    src/core/Token.cpp
    src/core/ErrorManager.cpp
    src/core/LexicalAnalyzer.cpp
    src/reports/ReportGenerator.cpp
)

# Archivos de cabecera
set(HEADERS
    include/core/Token.h
    include/core/ErrorManager.h
    include/core/LexicalAnalyzer.h
    include/reports/ReportGenerator.h
    include/gui/MainWindow.h
)

# Crear ejecutable
add_executable(MedLexer ${SOURCES} ${HEADERS})

# Enlazar Qt
target_link_libraries(MedLexer Qt6::Core Qt6::Widgets)

# Para Qt5 usar:
# target_link_libraries(MedLexer Qt5::Core Qt5::Widgets)