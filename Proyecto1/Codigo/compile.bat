@'
@echo off
echo ========================================
echo Compilando MedLexer
echo ========================================
echo.

g++ -std=c++17 -Iinclude src/main_gui.cpp src/gui/MainWindow.cpp src/core/Token.cpp src/core/ErrorManager.cpp src/core/LexicalAnalyzer.cpp src/reports/ReportGenerator.cpp -o MedLexer.exe

if %errorlevel% equ 0 (
    echo.
    echo COMPILACION EXITOSA!
    echo.
    MedLexer.exe
) else (
    echo.
    echo ERROR EN COMPILACION
    pause
)
'@ | Out-File -FilePath "compile.bat" -Encoding ASCII