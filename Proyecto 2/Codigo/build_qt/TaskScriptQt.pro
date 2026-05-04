QT += core widgets gui
CONFIG += c++17
CONFIG += release
TARGET = TaskScriptQt
TEMPLATE = app

SOURCES += \
    ../src/Lexer/Token.cpp \
    ../src/Lexer/LexicalAnalyzer.cpp \
    ../src/Error/Error.cpp \
    ../src/Error/ErrorManager.cpp \
    ../src/Parser/ASTNode.cpp \
    ../src/Parser/SyntaxAnalyzer.cpp \
    ../src/Reports/ReportGenerator.cpp \
    ../src/QtGUI/MainWindow.cpp \
    ../src/QtGUI/main.cpp

HEADERS += \
    ../src/Lexer/Token.h \
    ../src/Lexer/LexicalAnalyzer.h \
    ../src/Error/Error.h \
    ../src/Error/ErrorManager.h \
    ../src/Parser/ASTNode.h \
    ../src/Parser/SyntaxAnalyzer.h \
    ../src/Reports/ReportGenerator.h \
    ../src/QtGUI/MainWindow.h

INCLUDEPATH += ../src
