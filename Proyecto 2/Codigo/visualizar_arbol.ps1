# visualizar_arbol.ps1
param(
    [Parameter(Mandatory=$true)]
    [string]$ArchivoTask
)

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "  GENERADOR DE ARBOL DE DERIVACION" -ForegroundColor Cyan
Write-Host "  TaskScript - Lenguajes Formales" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

# Verificar que el archivo existe
if (-not (Test-Path $ArchivoTask)) {
    Write-Host "ERROR: No se encuentra el archivo: $ArchivoTask" -ForegroundColor Red
    exit 1
}

# Verificar que el ejecutable existe
if (-not (Test-Path "build\taskparser.exe")) {
    Write-Host "ERROR: No se encuentra build\taskparser.exe. Compile primero." -ForegroundColor Red
    exit 1
}

# Ejecutar el analizador
Write-Host "Analizando archivo: $ArchivoTask" -ForegroundColor Yellow
& ".\build\taskparser.exe" $ArchivoTask "arbol"

# Verificar si se generó el archivo DOT
$dotFile = "arbol_arbol.dot"
if (Test-Path $dotFile) {
    Write-Host ""
    Write-Host "Generando imagen del arbol de derivacion..." -ForegroundColor Yellow
    
    # Generar PNG
    $pngFile = "arbol_derivacion.png"
    dot -Tpng $dotFile -o $pngFile
    
    if ($LASTEXITCODE -eq 0) {
        Write-Host ""
        Write-Host "========================================" -ForegroundColor Green
        Write-Host "EXITO! Se generaron los siguientes archivos:" -ForegroundColor Green
        Write-Host "========================================" -ForegroundColor Green
        Write-Host ""
        Write-Host "  Reportes HTML:"
        Write-Host "    - arbol_kanban.html (Tablero Kanban)"
        Write-Host "    - arbol_responsable.html (Carga por responsable)"
        Write-Host "    - arbol_tokens.html (Tabla de tokens)"
        Write-Host ""
        Write-Host "  Arbol de derivacion:"
        Write-Host "    - arbol_arbol.dot (Codigo DOT)"
        Write-Host "    - arbol_derivacion.png (Imagen)"
        Write-Host ""
        
        # Abrir la imagen
        Write-Host "Abriendo imagen del arbol..." -ForegroundColor Yellow
        Invoke-Item $pngFile
        
        # Abrir el reporte Kanban
        Write-Host "Abriendo reporte Kanban..." -ForegroundColor Yellow
        Invoke-Item "arbol_kanban.html"
    } else {
        Write-Host "ERROR: No se pudo generar la imagen." -ForegroundColor Red
        Write-Host "Asegurese de tener Graphviz instalado." -ForegroundColor Red
    }
} else {
    Write-Host "ERROR: El analisis sintactico fallo. Revise el archivo .task" -ForegroundColor Red
}

Write-Host ""
Read-Host "Presione Enter para salir"