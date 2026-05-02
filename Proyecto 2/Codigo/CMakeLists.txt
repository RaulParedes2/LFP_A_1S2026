cmake_minimum_required(VERSION 3.16)
project(TaskScript VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Opciones de compilacion
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

# Archivos fuente
set(SOURCES
    src/Lexer/Token.cpp
    src/Lexer/LexicalAnalyzer.cpp
    src/Error/Error.cpp
    src/Error/ErrorManager.cpp
    src/Parser/ASTNode.cpp
    src/Parser/SyntaxAnalyzer.cpp
    src/Reports/ReportGenerator.cpp
    src/main.cpp
)

# Directorios de inclusion
include_directories(${CMAKE_SOURCE_DIR}/src)

# Crear ejecutable
add_executable(taskparser ${SOURCES})

# Para Windows: definir UNICODE
if(WIN32)
    add_definitions(-DUNICODE -D_UNICODE)
endif()

# Instalacion (opcional)
install(TARGETS taskparser DESTINATION bin)