#include "MainWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTableWidget>
#include <QTextEdit>
#include <QTabWidget>
#include <QSplitter>
#include <QStatusBar>
#include <QMenuBar>
#include <QToolBar>
#include <QAction>
#include <QFileDialog>
#include <QMessageBox>
#include <QHeaderView>
#include <QTextStream>
#include <QProcess>
#include <QDesktopServices>
#include <QApplication>
#include <fstream>
#include <cstdlib>
#include <iostream>

using namespace TaskScript;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ast(nullptr)
{
    setupUI();
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupUI()
{
    setWindowTitle("TaskScript - Analizador Lexico y Sintactico");
    setMinimumSize(1200, 800);

    // Crear widget central
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    // Layout principal
    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);

    // Crear splitter
    QSplitter *mainSplitter = new QSplitter(Qt::Horizontal);
    mainLayout->addWidget(mainSplitter);

    // ==================== PANEL IZQUIERDO ====================
    QWidget *leftPanel = new QWidget();
    QVBoxLayout *leftLayout = new QVBoxLayout(leftPanel);

    QLabel *editorLabel = new QLabel("Codigo Fuente TaskScript:");
    leftLayout->addWidget(editorLabel);

    textEditor = new QTextEdit();
    textEditor->setFont(QFont("Consolas", 10));
    leftLayout->addWidget(textEditor);

    leftPanel->setLayout(leftLayout);

    // ==================== PANEL DERECHO ====================
    QWidget *rightPanel = new QWidget();
    QVBoxLayout *rightLayout = new QVBoxLayout(rightPanel);

    // Crear Tab Widget
    tabWidget = new QTabWidget();

    // Pestaña 1: Tabla de Tokens
    QWidget *tokenPanel = new QWidget();
    QVBoxLayout *tokenLayout = new QVBoxLayout(tokenPanel);
    tokenTable = new QTableWidget();
    tokenTable->setColumnCount(5);
    QStringList tokenHeaders;
    tokenHeaders << "No." << "Lexema" << "Tipo" << "Linea" << "Columna";
    tokenTable->setHorizontalHeaderLabels(tokenHeaders);
    tokenTable->setAlternatingRowColors(true);
    tokenLayout->addWidget(tokenTable);
    tokenPanel->setLayout(tokenLayout);
    tabWidget->addTab(tokenPanel, "Tabla de Tokens");

    // Pestaña 2: Tabla de Errores
    QWidget *errorPanel = new QWidget();
    QVBoxLayout *errorLayout = new QVBoxLayout(errorPanel);
    errorTable = new QTableWidget();
    errorTable->setColumnCount(7);
    QStringList errorHeaders;
    errorHeaders << "No." << "Lexema" << "Tipo" << "Descripcion" << "Linea" << "Columna" << "Gravedad";
    errorTable->setHorizontalHeaderLabels(errorHeaders);
    errorTable->setAlternatingRowColors(true);
    errorLayout->addWidget(errorTable);
    errorPanel->setLayout(errorLayout);
    tabWidget->addTab(errorPanel, "Tabla de Errores");

    // Pestaña 3: Codigo DOT
    QWidget *dotPanel = new QWidget();
    QVBoxLayout *dotLayout = new QVBoxLayout(dotPanel);
    dotDisplay = new QTextEdit();
    dotDisplay->setFont(QFont("Consolas", 9));
    dotDisplay->setReadOnly(true);
    dotLayout->addWidget(dotDisplay);
    dotPanel->setLayout(dotLayout);
    tabWidget->addTab(dotPanel, "Codigo DOT");

    rightLayout->addWidget(tabWidget);

    // Panel de Botones
    QWidget *buttonPanel = new QWidget();
    QHBoxLayout *buttonLayout = new QHBoxLayout(buttonPanel);

    btnLoad = new QPushButton("Cargar Archivo");
    btnAnalyze = new QPushButton("Analizar");
    btnReports = new QPushButton("Reportes HTML");
    btnSaveDot = new QPushButton("Guardar DOT");
    btnViewGraph = new QPushButton("Ver Grafico");
    btnClear = new QPushButton("Limpiar");

    btnLoad->setMinimumHeight(35);
    btnAnalyze->setMinimumHeight(35);
    btnReports->setMinimumHeight(35);
    btnSaveDot->setMinimumHeight(35);
    btnViewGraph->setMinimumHeight(35);
    btnClear->setMinimumHeight(35);

    buttonLayout->addWidget(btnLoad);
    buttonLayout->addWidget(btnAnalyze);
    buttonLayout->addWidget(btnReports);
    buttonLayout->addWidget(btnSaveDot);
    buttonLayout->addWidget(btnViewGraph);
    buttonLayout->addWidget(btnClear);

    buttonPanel->setLayout(buttonLayout);
    rightLayout->addWidget(buttonPanel);

    rightPanel->setLayout(rightLayout);

    // Agregar paneles al splitter
    mainSplitter->addWidget(leftPanel);
    mainSplitter->addWidget(rightPanel);
    mainSplitter->setStretchFactor(0, 1);
    mainSplitter->setStretchFactor(1, 2);

    // Barra de estado
    statusBar = new QStatusBar();
    setStatusBar(statusBar);
    statusBar->showMessage("Listo. Cargue un archivo .task para comenzar.");

    // Crear menu bar
    QMenuBar *menuBar = new QMenuBar(this);
    setMenuBar(menuBar);

    QMenu *fileMenu = menuBar->addMenu("Archivo");
    QAction *loadAction = fileMenu->addAction("Cargar Archivo");
    fileMenu->addSeparator();
    QAction *clearAction = fileMenu->addAction("Limpiar");
    fileMenu->addSeparator();
    QAction *exitAction = fileMenu->addAction("Salir");

    QMenu *analyzeMenu = menuBar->addMenu("Analisis");
    QAction *analyzeAction = analyzeMenu->addAction("Analizar");
    analyzeMenu->addSeparator();
    QAction *reportsAction = analyzeMenu->addAction("Generar Reportes");
    QAction *dotAction = analyzeMenu->addAction("Guardar DOT");

    // Conectar acciones del menu
    connect(loadAction, &QAction::triggered, this, &MainWindow::onLoadFile);
    connect(clearAction, &QAction::triggered, this, &MainWindow::onClearData);
    connect(exitAction, &QAction::triggered, this, &MainWindow::onExit);
    connect(analyzeAction, &QAction::triggered, this, &MainWindow::onAnalyze);
    connect(reportsAction, &QAction::triggered, this, &MainWindow::onGenerateReports);
    connect(dotAction, &QAction::triggered, this, &MainWindow::onSaveDot);

    // Conectar botones
    connect(btnLoad, &QPushButton::clicked, this, &MainWindow::onLoadFile);
    connect(btnAnalyze, &QPushButton::clicked, this, &MainWindow::onAnalyze);
    connect(btnReports, &QPushButton::clicked, this, &MainWindow::onGenerateReports);
    connect(btnSaveDot, &QPushButton::clicked, this, &MainWindow::onSaveDot);
    connect(btnViewGraph, &QPushButton::clicked, this, &MainWindow::onViewGraph);
    connect(btnClear, &QPushButton::clicked, this, &MainWindow::onClearData);
}

void MainWindow::onLoadFile()
{
    try
    {
        QString filename = QFileDialog::getOpenFileName(this,
                                                        "Seleccione un archivo .task",
                                                        "",
                                                        "Archivos TaskScript (*.task);;Todos los archivos (*.*)");

        if (filename.isEmpty())
            return;

        currentFilename = filename;

        QFile file(filename);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QTextStream stream(&file);
            textEditor->setPlainText(stream.readAll());
            file.close();
            statusBar->showMessage("Archivo cargado: " + filename);
            QMessageBox::information(this, "Exito", "Archivo cargado exitosamente.");
        }
        else
        {
            QMessageBox::warning(this, "Error", "No se pudo abrir el archivo.");
        }
    }
    catch (const std::exception &e)
    {
        QMessageBox::critical(this, "Error", QString("Error al cargar: %1").arg(e.what()));
    }
}

void MainWindow::onClearData()
{
    QMessageBox::information(this, "Debug", "Limpiando datos...");

    textEditor->clear();
    clearTables();
    tokens.clear();
    ast.reset();
    lexErrors.clear();
    syntaxErrors.clear();
    dotCode.clear();
    currentFilename.clear();
    statusBar->showMessage("Datos limpiados.");

    QMessageBox::information(this, "Debug", "Datos limpiados correctamente");
}
void MainWindow::onAnalyze()
{
    try
    {
        if (textEditor->toPlainText().isEmpty())
        {
            QMessageBox::warning(this, "Advertencia", "Primero cargue o escriba un archivo .task");
            return;
        }

        clearTables();

        // Guardar contenido a archivo temporal
        QString tempFile = "temp_analysis.task";
        QFile file(tempFile);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            QTextStream stream(&file);
            stream << textEditor->toPlainText();
            file.close();
        }

        // Analisis lexico
        lexErrors.clear();
        LexicalAnalyzer lexer(lexErrors);

        if (!lexer.loadFile(tempFile.toStdString()))
        {
            QMessageBox::warning(this, "Error", "Error al cargar el archivo temporal.");
            QFile::remove(tempFile);
            return;
        }

        // Obtener tokens
        tokens.clear();
        Token token = lexer.getNextToken();
        while (token.getType() != TokenType::FIN_DE_ARCHIVO)
        {
            tokens.push_back(token);
            token = lexer.getNextToken();
        }

        updateTokenTable();

        // Analisis sintactico
        syntaxErrors.clear();
        lexer.reset();
        SyntaxAnalyzer parser(lexer, syntaxErrors);
        ast = parser.parse();

        updateErrorTable();

        if (ast != nullptr)
        {
            dotCode = QString::fromStdString(parser.generateDotCode());
            updateDotDisplay();
            statusBar->showMessage(QString("Analisis exitoso. Tokens: %1").arg(tokens.size()));
            QMessageBox::information(this, "Exito", "Analisis completado exitosamente.");
        }
        else
        {
            statusBar->showMessage(QString("Analisis con errores. Errores: %1").arg(syntaxErrors.getErrorCount()));
            if (syntaxErrors.getErrorCount() > 0)
            {
                QMessageBox::warning(this, "Errores", QString("Se encontraron %1 errores sintacticos.").arg(syntaxErrors.getErrorCount()));
            }
        }

        QFile::remove(tempFile);
    }
    catch (const std::exception &e)
    {
        QMessageBox::critical(this, "Error", QString("Error durante el analisis: %1").arg(e.what()));
    }
}

void MainWindow::onGenerateReports()
{
    QMessageBox::information(this, "Debug", "Entrando a onGenerateReports");

    if (tokens.empty() || ast == nullptr)
    {
        QMessageBox::warning(this, "Advertencia", "Primero debe analizar un archivo valido.");
        return;
    }

    QMessageBox::information(this, "Debug", "Tokens y AST válidos");

    QString baseName = QFileDialog::getSaveFileName(this,
                                                    "Guardar reportes (base del nombre)",
                                                    "reporte",
                                                    "Archivos HTML (*.html)");

    if (baseName.isEmpty())
        return;

    QMessageBox::information(this, "Debug", "Base name: " + baseName);

    if (baseName.endsWith(".html"))
    {
        baseName.chop(5);
    }

    QMessageBox::information(this, "Debug", "Creando ReportGenerator");

    try
    {
        ReportGenerator reporter;
        QMessageBox::information(this, "Debug", "ReportGenerator creado");

        reporter.loadData(tokens, syntaxErrors.getAllErrors());
        QMessageBox::information(this, "Debug", "Datos cargados");

        reporter.loadAST(ast.get());
        QMessageBox::information(this, "Debug", "AST cargado");

        bool result = reporter.saveAllReports(baseName.toStdString());
        QMessageBox::information(this, "Debug", QString("saveAllReports resultado: %1").arg(result));

        if (result)
        {
            QMessageBox::information(this, "Exito",
                                     "Reportes generados:\n" + baseName + "_kanban.html\n" +
                                         baseName + "_responsable.html\n" + baseName + "_tokens.html");
        }
        else
        {
            QMessageBox::warning(this, "Error", "Error al generar los reportes.");
        }
    }
    catch (const std::exception &e)
    {
        QMessageBox::critical(this, "Error", QString("Excepción: %1").arg(e.what()));
    }
    catch (...)
    {
        QMessageBox::critical(this, "Error", "Excepción desconocida");
    }

    QMessageBox::information(this, "Debug", "Saliendo de onGenerateReports");
}

void MainWindow::onSaveDot()
{
    try
    {
        if (dotCode.isEmpty())
        {
            QMessageBox::warning(this, "Advertencia", "No hay codigo DOT para guardar.");
            return;
        }

        QString filename = QFileDialog::getSaveFileName(this,
                                                        "Guardar archivo DOT",
                                                        "arbol_derivacion.dot",
                                                        "Archivos DOT (*.dot);;Todos los archivos (*.*)");

        if (filename.isEmpty())
            return;

        QFile file(filename);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            QTextStream stream(&file);
            stream << dotCode;
            file.close();
            QMessageBox::information(this, "Exito", "Archivo DOT guardado.");
        }
        else
        {
            QMessageBox::warning(this, "Error", "Error al guardar.");
        }
    }
    catch (const std::exception &e)
    {
        QMessageBox::critical(this, "Error", QString("Error al guardar DOT: %1").arg(e.what()));
    }
}

void MainWindow::onViewGraph()
{
    try
    {
        if (dotCode.isEmpty())
        {
            QMessageBox::warning(this, "Advertencia", "No hay arbol para visualizar.");
            return;
        }

        QString dotFile = "temp_arbol.dot";
        QFile file(dotFile);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            QMessageBox::warning(this, "Error", "Error al crear archivo temporal.");
            return;
        }
        QTextStream stream(&file);
        stream << dotCode;
        file.close();

        QProcess process;
        process.start("dot", QStringList() << "-Tpng" << dotFile << "-o" << "temp_arbol.png");
        process.waitForFinished();

        if (process.exitCode() == 0)
        {
            QDesktopServices::openUrl(QUrl::fromLocalFile("temp_arbol.png"));
        }
        else
        {
            QMessageBox::warning(this, "Error", "Graphviz no instalado o no encontrado.");
        }
    }
    catch (const std::exception &e)
    {
        QMessageBox::critical(this, "Error", QString("Error al ver grafico: %1").arg(e.what()));
    }
}

void MainWindow::onExit()
{
    close();
}

void MainWindow::updateTokenTable()
{
    tokenTable->setRowCount(0);
    tokenTable->setRowCount((int)tokens.size());

    for (int i = 0; i < (int)tokens.size(); i++)
    {
        tokenTable->setItem(i, 0, new QTableWidgetItem(QString::number(i + 1)));
        tokenTable->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(tokens[i].getLexeme())));
        tokenTable->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(tokens[i].typeToString())));
        tokenTable->setItem(i, 3, new QTableWidgetItem(QString::number(tokens[i].getLine())));
        tokenTable->setItem(i, 4, new QTableWidgetItem(QString::number(tokens[i].getColumn())));
    }
    tokenTable->resizeColumnsToContents();
}

void MainWindow::updateErrorTable()
{
    const auto &errors = syntaxErrors.getAllErrors();
    errorTable->setRowCount(0);
    errorTable->setRowCount((int)errors.size());

    for (int i = 0; i < (int)errors.size(); i++)
    {
        errorTable->setItem(i, 0, new QTableWidgetItem(QString::number(errors[i].getId())));
        errorTable->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(errors[i].getLexeme())));
        errorTable->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(errors[i].getTypeString())));
        errorTable->setItem(i, 3, new QTableWidgetItem(QString::fromStdString(errors[i].getDescription())));
        errorTable->setItem(i, 4, new QTableWidgetItem(QString::number(errors[i].getLine())));
        errorTable->setItem(i, 5, new QTableWidgetItem(QString::number(errors[i].getColumn())));
        errorTable->setItem(i, 6, new QTableWidgetItem(QString::fromStdString(errors[i].getSeverityString())));
    }
    errorTable->resizeColumnsToContents();
}

void MainWindow::updateDotDisplay()
{
    dotDisplay->setPlainText(dotCode);
}

void MainWindow::clearTables()
{
    tokenTable->setRowCount(0);
    errorTable->setRowCount(0);
    dotDisplay->clear();
}