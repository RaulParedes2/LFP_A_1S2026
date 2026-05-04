# compilar_con_qmake.ps1
param(
    [string]$QtPath = "C:\Qt\6.11.0\mingw_64",
    [string]$MingwPath = "C:\Qt\Tools\mingw1310_64"
)

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "  COMPILANDO TASKSCRIPT CON QMAKE" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

# Verificar rutas
if (-not (Test-Path $QtPath)) {
    Write-Host "ERROR: No se encuentra Qt en: $QtPath" -ForegroundColor Red
    exit 1
}

# Configurar PATH
$env:PATH = "$QtPath\bin;$MingwPath\bin;$env:PATH"

# Crear directorio build
if (-not (Test-Path "build_qt")) {
    New-Item -ItemType Directory -Path "build_qt" | Out-Null
}

cd build_qt

# Crear archivo .pro para qmake
$proContent = @"
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
"@

Set-Content -Path "TaskScriptQt.pro" -Value $proContent

Write-Host "Ejecutando qmake..." -ForegroundColor Yellow
qmake TaskScriptQt.pro

if ($LASTEXITCODE -ne 0) {
    Write-Host "ERROR: qmake fallo" -ForegroundColor Red
    cd ..
    exit 1
}

Write-Host ""
Write-Host "Compilando con mingw32-make..." -ForegroundColor Yellow
mingw32-make

if ($LASTEXITCODE -ne 0) {
    Write-Host "ERROR: La compilacion fallo" -ForegroundColor Red
    cd ..
    exit 1
}

cd ..

Write-Host ""
Write-Host "========================================" -ForegroundColor Green
Write-Host "  COMPILACION EXITOSA!" -ForegroundColor Green
Write-Host "========================================" -ForegroundColor Green

# Buscar el ejecutable
$exe = Get-ChildItem -Path "build_qt" -Filter "*.exe" -Recurse | Select-Object -First 1

if ($exe) {
    Write-Host ""
    Write-Host "Ejecutable: $($exe.FullName)" -ForegroundColor Yellow
    Write-Host ""
    Write-Host "Ejecutando TaskScript GUI..." -ForegroundColor Cyan
    & $exe.FullName
} else {
    Write-Host "No se encontro el ejecutable" -ForegroundColor Red
}