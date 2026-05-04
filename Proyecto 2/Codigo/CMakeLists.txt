cmake_minimum_required(VERSION 3.20)
project(TaskScriptQt VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)  # Automatic MOC for Qt
set(CMAKE_AUTORCC ON)  # Automatic RCC for resources
set(CMAKE_AUTOUIC ON)  # Automatic UIC for UI files

# Find Qt6
find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

# Source files
set(SOURCES
    src/Lexer/Token.cpp
    src/Lexer/LexicalAnalyzer.cpp
    src/Error/Error.cpp
    src/Error/ErrorManager.cpp
    src/Parser/ASTNode.cpp
    src/Parser/SyntaxAnalyzer.cpp
    src/Reports/ReportGenerator.cpp
    src/QtGUI/MainWindow.cpp
    src/QtGUI/main.cpp
)

# Header files (for MOC)
set(HEADERS
    src/Lexer/Token.h
    src/Lexer/LexicalAnalyzer.h
    src/Error/Error.h
    src/Error/ErrorManager.h
    src/Parser/ASTNode.h
    src/Parser/SyntaxAnalyzer.h
    src/Reports/ReportGenerator.h
    src/QtGUI/MainWindow.h
)

# Create executable
add_executable(TaskScriptQt WIN32 ${SOURCES} ${HEADERS})

# Link Qt libraries
target_link_libraries(TaskScriptQt Qt6::Core Qt6::Widgets)

# Include directories
target_include_directories(TaskScriptQt PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Set properties for Windows
if(WIN32)
    set_target_properties(TaskScriptQt PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()