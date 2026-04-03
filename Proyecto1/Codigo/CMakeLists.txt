
cmake_minimum_required(VERSION 3.16)
project(MedLexer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Buscar Qt6
find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

# Incluir directorios
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${Qt6_INCLUDE_DIRS})

# Archivos fuente - CORREGIDO (sin main_gui_debug.cpp)
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
    include/gui/MainWindow.h
)

# Crear ejecutable
add_executable(MedLexer WIN32 ${SOURCES} ${HEADERS})

# Enlazar Qt
target_link_libraries(MedLexer Qt6::Core Qt6::Widgets)

# Para que MOC procese los archivos de cabecera
set_target_properties(MedLexer PROPERTIES AUTOMOC ON)
