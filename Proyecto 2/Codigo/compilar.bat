@echo off
echo Compilando TaskScript...

g++ -std=c++17 -Wall -Wextra -I src src\Lexer\Token.cpp src\Lexer\LexicalAnalyzer.cpp src\Error\Error.cpp src\Error\ErrorManager.cpp src\Parser\ASTNode.cpp src\Parser\SyntaxAnalyzer.cpp src\Reports\ReportGenerator.cpp src\main.cpp -o build\taskparser.exe

if %errorlevel% == 0 (
    echo Compilacion exitosa!
    echo Ejecutable: build\taskparser.exe
) else (
    echo Error en la compilacion
)

pause