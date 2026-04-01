@echo off
echo ========================================
echo Compilando MedLexer con Qt
echo ========================================
echo.

REM CAMBIAR ESTA RUTA SEGUN TU VERSION DE QT
set QT_DIR=C:\Qt\6.11.0\mingw_64
set QT_INCLUDE=%QT_DIR%\include
set QT_LIB=%QT_DIR%\lib

REM Compilar todos los archivos
echo Compilando...
g++ -std=c++17 ^
    -I include ^
    -I %QT_INCLUDE% ^
    -I %QT_INCLUDE%\QtCore ^
    -I %QT_INCLUDE%\QtWidgets ^
    -I %QT_INCLUDE%\QtGui ^
    src/main_gui.cpp ^
    src/gui/MainWindow.cpp ^
    src/core/Token.cpp ^
    src/core/ErrorManager.cpp ^
    src/core/LexicalAnalyzer.cpp ^
    src/reports/ReportGenerator.cpp ^
    -L %QT_LIB% ^
    -lQt6Core ^
    -lQt6Widgets ^
    -lQt6Gui ^
    -o MedLexer_Qt.exe

if %errorlevel% equ 0 (
    echo.
    echo ========================================
    echo COMPILACION EXITOSA!
    echo ========================================
    echo.
    echo Ejecutando MedLexer...
    MedLexer_Qt.exe
) else (
    echo.
    echo ========================================
    echo ERROR EN LA COMPILACION
    echo ========================================
    echo Revisa los errores arriba
    pause
)