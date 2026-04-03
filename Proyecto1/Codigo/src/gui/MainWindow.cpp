#include "../../include/gui/MainWindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QDesktopServices>
#include <QUrl>
#include <QHeaderView>
#include <QFile>
#include <QTextStream>
#include <iostream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    std::cout << "MainWindow constructor iniciado" << std::endl;

    errorManager = new ErrorManager();
    lexer = new LexicalAnalyzer(errorManager);
    reportGen = new ReportGenerator();

    std::cout << "Componentes creados" << std::endl;

    setupUI();
    setupConnections();

    std::cout << "UI configurada" << std::endl;

    setWindowTitle("MedLexer - Analizador Lexico para Gestion Hospitalaria");
    resize(1200, 800);

    std::cout << "MainWindow constructor finalizado" << std::endl;
}

MainWindow::~MainWindow()
{
    delete lexer;
    delete errorManager;
    delete reportGen;
}

void MainWindow::setupUI()
{
    std::cout << "setupUI iniciado" << std::endl;

    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    QHBoxLayout *buttonLayout = new QHBoxLayout();

    loadButton = new QPushButton("Cargar Archivo .med");
    analyzeButton = new QPushButton("Analizar");
    clearButton = new QPushButton("Limpiar");

    report1Button = new QPushButton("Reporte 1: Pacientes");
    report2Button = new QPushButton("Reporte 2: Medicos");
    report3Button = new QPushButton("Reporte 3: Citas");
    report4Button = new QPushButton("Reporte 4: Estadisticas");

    buttonLayout->addWidget(loadButton);
    buttonLayout->addWidget(analyzeButton);
    buttonLayout->addWidget(clearButton);
    buttonLayout->addStretch();
    buttonLayout->addWidget(report1Button);
    buttonLayout->addWidget(report2Button);
    buttonLayout->addWidget(report3Button);
    buttonLayout->addWidget(report4Button);

    mainLayout->addLayout(buttonLayout);

    QSplitter *mainSplitter = new QSplitter(Qt::Horizontal);

    QWidget *editorPanel = new QWidget();
    QVBoxLayout *editorLayout = new QVBoxLayout(editorPanel);

    QLabel *editorLabel = new QLabel("Codigo Fuente (MedLang):");
    editorLayout->addWidget(editorLabel);

    sourceCodeEditor = new QTextEdit();
    sourceCodeEditor->setFont(QFont("Consolas", 10));
    editorLayout->addWidget(sourceCodeEditor);

    mainSplitter->addWidget(editorPanel);

    QWidget *resultsPanel = new QWidget();
    QVBoxLayout *resultsLayout = new QVBoxLayout(resultsPanel);

    QLabel *resultsLabel = new QLabel("Resultados del Analisis:");
    resultsLayout->addWidget(resultsLabel);

    tabWidget = new QTabWidget();

    tokenTable = new QTableWidget();
    tokenTable->setColumnCount(5);
    tokenTable->setHorizontalHeaderLabels({"#", "Lexema", "Tipo", "Linea", "Columna"});
    tokenTable->horizontalHeader()->setStretchLastSection(true);
    tabWidget->addTab(tokenTable, "Tokens");

    errorTable = new QTableWidget();
    errorTable->setColumnCount(6);
    errorTable->setHorizontalHeaderLabels({"#", "Lexema", "Tipo Error", "Descripcion", "Linea", "Columna"});
    errorTable->horizontalHeader()->setStretchLastSection(true);
    tabWidget->addTab(errorTable, "Errores");

    resultsLayout->addWidget(tabWidget);

    mainSplitter->addWidget(resultsPanel);
    mainSplitter->setSizes({600, 600});

    mainLayout->addWidget(mainSplitter);

    statusLabel = new QLabel("Listo. Carga un archivo .med para comenzar.");
    mainLayout->addWidget(statusLabel);

    std::cout << "setupUI finalizado" << std::endl;
}

void MainWindow::setupConnections()
{
    connect(loadButton, &QPushButton::clicked, this, &MainWindow::onLoadFile);
    connect(analyzeButton, &QPushButton::clicked, this, &MainWindow::onAnalyze);
    connect(clearButton, &QPushButton::clicked, this, &MainWindow::onClear);
    connect(report1Button, &QPushButton::clicked, this, &MainWindow::onViewReport1);
    connect(report2Button, &QPushButton::clicked, this, &MainWindow::onViewReport2);
    connect(report3Button, &QPushButton::clicked, this, &MainWindow::onViewReport3);
    connect(report4Button, &QPushButton::clicked, this, &MainWindow::onViewReport4);
}

void MainWindow::onLoadFile()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    "Abrir archivo MedLang", "",
                                                    "Archivos MedLang (*.med);;Todos los archivos (*)");

    if (fileName.isEmpty())
        return;

    currentFile = fileName;

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::warning(this, "Error", "No se pudo abrir el archivo.");
        return;
    }

    QTextStream in(&file);
    sourceCodeEditor->setText(in.readAll());
    file.close();

    updateStatus(QString("Archivo cargado: %1").arg(fileName));
    clearTables();
}

void MainWindow::onAnalyze()
{
    QString code = sourceCodeEditor->toPlainText();
    if (code.isEmpty())
    {
        QMessageBox::warning(this, "Advertencia", "No hay codigo para analizar.");
        return;
    }

    updateStatus("Analizando codigo...");

    errorManager->clearErrors();
    reportGen->clear();

    lexer->setSourceCode(code.toStdString());
    std::vector<Token> tokens = lexer->analyze();

    // Mostrar información de depuración
    std::cout << "=== ANALISIS COMPLETADO ===" << std::endl;
    std::cout << "Total tokens: " << tokens.size() << std::endl;
    std::cout << "Total errores: " << errorManager->getErrorCount() << std::endl;

    // Extraer datos de los tokens
    extractDataFromTokens(tokens);

    displayTokens(tokens);
    displayErrors();

    // Mostrar datos extraídos
    std::cout << "=== DATOS EXTRAIDOS ===" << std::endl;
    std::cout << "Pacientes: " << reportGen->getPatients().size() << std::endl;
    for (const auto &p : reportGen->getPatients())
    {
        std::cout << "  - " << p.name << " (" << p.age << ", " << p.bloodType << ")" << std::endl;
    }
    std::cout << "Medicos: " << reportGen->getDoctors().size() << std::endl;
    for (const auto &d : reportGen->getDoctors())
    {
        std::cout << "  - " << d.name << " (" << d.specialty << ", " << d.code << ")" << std::endl;
    }
    std::cout << "Citas: " << reportGen->getAppointments().size() << std::endl;

    // Crear carpeta reports
    QDir dir;
    if (!dir.exists("reports"))
    {
        dir.mkdir("reports");
    }

    // Generar reportes
    bool reportsGenerated = reportGen->generateAllReports("./reports/");
    bool graphvizGenerated = reportGen->generateGraphviz("hospital.dot");

    if (reportsGenerated)
    {
        std::cout << "Reportes generados en ./reports/" << std::endl;
        updateStatus(QString("Reportes generados. Pacientes: %1, Medicos: %2, Citas: %3")
                         .arg(reportGen->getPatients().size())
                         .arg(reportGen->getDoctors().size())
                         .arg(reportGen->getAppointments().size()));
    }
    else
    {
        std::cout << "ERROR: No se pudieron generar los reportes" << std::endl;
        updateStatus("Error al generar reportes", true);
    }

    if (graphvizGenerated)
    {
        std::cout << "Diagrama Graphviz generado: hospital.dot" << std::endl;
    }

    int errorCount = errorManager->getErrorCount();
    if (errorCount > 0)
    {
        updateStatus(QString("Analisis con %1 errores. Reportes generados con datos validos.").arg(errorCount), true);
    }
    else
    {
        updateStatus(QString("Analisis completado. %1 tokens.").arg(tokens.size()));
    }
}

void MainWindow::extractDataFromTokens(const std::vector<Token> &tokens)
{

    std::cout << "=== EXTRACTANDO DATOS ===" << std::endl;
    std::cout << "Total tokens a procesar: " << tokens.size() << std::endl;
    std::map<std::string, Patient> patientMap;
    std::map<std::string, Doctor> doctorMap;
    std::vector<Appointment> appointmentsList;

    Patient currentPatient;
    Doctor currentDoctor;
    Appointment currentAppointment;

    bool inPatients = false, inDoctors = false, inAppointments = false, inDiagnostics = false;
    std::string currentDiagnosticPatient;
    Patient diagnosticPatient;

    for (size_t i = 0; i < tokens.size(); i++)
    {
        Token t = tokens[i];

        // Detectar secciones
        if (t.getType() == TokenType::PACIENTES)
        {
            inPatients = true;
            inDoctors = false;
            inAppointments = false;
            inDiagnostics = false;
            continue;
        }
        else if (t.getType() == TokenType::MEDICOS)
        {
            inPatients = false;
            inDoctors = true;
            inAppointments = false;
            inDiagnostics = false;
            continue;
        }
        else if (t.getType() == TokenType::CITAS)
        {
            inPatients = false;
            inDoctors = false;
            inAppointments = true;
            inDiagnostics = false;
            continue;
        }
        else if (t.getType() == TokenType::DIAGNOSTICOS)
        {
            inPatients = false;
            inDoctors = false;
            inAppointments = false;
            inDiagnostics = true;
            continue;
        }

        // Procesar paciente - buscar "paciente:" seguido de nombre
        if (inPatients && t.getType() == TokenType::PACIENTE)
        {
            currentPatient = Patient();
            // Buscar el nombre después de :
            for (size_t j = i + 1; j < tokens.size() && j < i + 10; j++)
            {
                if (tokens[j].getType() == TokenType::COLON && j + 1 < tokens.size())
                {
                    if (tokens[j + 1].getType() == TokenType::STRING)
                    {
                        currentPatient.name = tokens[j + 1].getLexeme();
                        break;
                    }
                }
            }
        }

        // Procesar médico
        if (inDoctors && t.getType() == TokenType::MEDICO)
        {
            currentDoctor = Doctor();
            for (size_t j = i + 1; j < tokens.size() && j < i + 10; j++)
            {
                if (tokens[j].getType() == TokenType::COLON && j + 1 < tokens.size())
                {
                    if (tokens[j + 1].getType() == TokenType::STRING)
                    {
                        currentDoctor.name = tokens[j + 1].getLexeme();
                        break;
                    }
                }
            }
        }

        // Procesar diagnóstico - buscar "diagnostico:" seguido de nombre
        if (inDiagnostics && t.getType() == TokenType::DIAGNOSTICO)
        {
            diagnosticPatient = Patient();
            // Buscar el nombre del paciente después de :
            for (size_t j = i + 1; j < tokens.size() && j < i + 10; j++)
            {
                if (tokens[j].getType() == TokenType::COLON && j + 1 < tokens.size())
                {
                    if (tokens[j + 1].getType() == TokenType::STRING)
                    {
                        diagnosticPatient.name = tokens[j + 1].getLexeme();
                        currentDiagnosticPatient = tokens[j + 1].getLexeme();
                        break;
                    }
                }
            }
        }

        // Procesar atributos
        if (t.getType() == TokenType::ATRIBUTO && i + 2 < tokens.size() && tokens[i + 1].getType() == TokenType::COLON)
        {
            std::string attr = t.getLexeme();
            std::string value = tokens[i + 2].getLexeme();

            if (inPatients && !currentPatient.name.empty())
            {
                if (attr == "edad" && tokens[i + 2].getType() == TokenType::INTEGER)
                {
                    currentPatient.age = std::stoi(value);
                }
                else if (attr == "tipo_sangre" && (tokens[i + 2].getType() == TokenType::STRING || tokens[i + 2].getType() == TokenType::BLOOD_TYPE))
                {
                    string bloodValue = value;
                    // Validar tipo de sangre
                    if (bloodValue == "A+" || bloodValue == "A-" || bloodValue == "B+" || bloodValue == "B-" ||
                        bloodValue == "O+" || bloodValue == "O-" || bloodValue == "AB+" || bloodValue == "AB-")
                    {
                        currentPatient.bloodType = bloodValue;
                        patientMap[currentPatient.name] = currentPatient;
                    }
                    else
                    {
                        errorManager->addError(bloodValue, "Tipo de sangre invalido",
                                               "El valor '" + bloodValue + "' no es un tipo de sangre valido. Valores validos: O+, O-, A+, A-, B+, B-, AB+, AB-",
                                               tokens[i + 2].getLine(), tokens[i + 2].getColumn(), ErrorSeverity::ERROR);
                    }
                }
            }
            else if (inDoctors && !currentDoctor.name.empty())
            {
                if (attr == "especialidad")
                {
                    currentDoctor.specialty = value;
                }
                else if (attr == "codigo" && tokens[i + 2].getType() == TokenType::STRING)
                {
                    currentDoctor.code = value;
                    doctorMap[currentDoctor.name] = currentDoctor;
                }
            }
            else if (inAppointments)
            {
                if (attr == "fecha" && tokens[i + 2].getType() == TokenType::DATE)
                {
                    currentAppointment.date = value;
                }
                else if (attr == "hora" && tokens[i + 2].getType() == TokenType::TIME)
                {
                    currentAppointment.time = value;
                }
            }
            else if (inDiagnostics && !currentDiagnosticPatient.empty())
            {
                if (attr == "condicion" && tokens[i + 2].getType() == TokenType::STRING)
                {
                    diagnosticPatient.diagnosis = value;
                    diagnosticPatient.hasDiagnosis = true;
                }
                else if (attr == "medicamento" && tokens[i + 2].getType() == TokenType::STRING)
                {
                    diagnosticPatient.medication = value;
                }
                else if (attr == "dosis")
                {
                    diagnosticPatient.dosis = value;
                    // Asignar diagnóstico al paciente correcto
                    if (patientMap.find(currentDiagnosticPatient) != patientMap.end())
                    {
                        patientMap[currentDiagnosticPatient].hasDiagnosis = diagnosticPatient.hasDiagnosis;
                        patientMap[currentDiagnosticPatient].diagnosis = diagnosticPatient.diagnosis;
                        patientMap[currentDiagnosticPatient].medication = diagnosticPatient.medication;
                        patientMap[currentDiagnosticPatient].dosis = diagnosticPatient.dosis;
                    }
                    else
                    {
                        // Si el paciente no existe aún, crearlo con el diagnóstico
                        diagnosticPatient.name = currentDiagnosticPatient;
                        patientMap[currentDiagnosticPatient] = diagnosticPatient;
                    }
                    currentDiagnosticPatient.clear();
                }
            }
        }

        // Procesar citas con "con"
        if (inAppointments && t.getType() == TokenType::CON)
        {
            // Buscar paciente (antes de "con")
            for (int j = i - 1; j >= 0 && j > (int)i - 10; j--)
            {
                if (tokens[j].getType() == TokenType::STRING && currentAppointment.patient.empty())
                {
                    currentAppointment.patient = tokens[j].getLexeme();
                }
            }
            // Buscar médico (después de "con")
            for (size_t j = i + 1; j < tokens.size() && j < i + 10; j++)
            {
                if (tokens[j].getType() == TokenType::STRING && currentAppointment.doctor.empty())
                {
                    currentAppointment.doctor = tokens[j].getLexeme();
                    break;
                }
            }
        }

        // Guardar cita
        if (inAppointments && t.getType() == TokenType::RBRACKET &&
            !currentAppointment.patient.empty() && !currentAppointment.doctor.empty())
        {
            // Buscar especialidad del médico
            if (doctorMap.find(currentAppointment.doctor) != doctorMap.end())
            {
                currentAppointment.specialty = doctorMap[currentAppointment.doctor].specialty;
            }
            appointmentsList.push_back(currentAppointment);
            currentAppointment = Appointment();
        }
    }

    // Agregar todos los datos al reportGen
    for (const auto &pair : patientMap)
    {
        reportGen->addPatient(pair.second);
    }
    for (const auto &pair : doctorMap)
    {
        reportGen->addDoctor(pair.second);
    }
    for (const auto &a : appointmentsList)
    {
        reportGen->addAppointment(a);
    }

    // Actualizar conteo de citas por médico
    std::vector<Doctor> doctors = reportGen->getDoctors();
    for (auto &d : doctors)
    {
        d.appointments = 0;
        d.patients.clear();
        for (const auto &a : reportGen->getAppointments())
        {
            if (a.doctor == d.name)
            {
                d.appointments++;
                d.patients.push_back(a.patient);
            }
        }
    }

    // Limpiar y volver a agregar con datos actualizados
    std::vector<Patient> patients = reportGen->getPatients();
    std::vector<Appointment> appointments = reportGen->getAppointments();
    reportGen->clear();
    for (const auto &p : patients)
        reportGen->addPatient(p);
    for (const auto &d : doctors)
        reportGen->addDoctor(d);
    for (const auto &a : appointments)
        reportGen->addAppointment(a);

    std::cout << "Pacientes encontrados: " << patientMap.size() << std::endl;
    std::cout << "Medicos encontrados: " << doctorMap.size() << std::endl;
    std::cout << "Citas encontradas: " << appointmentsList.size() << std::endl;
}
void MainWindow::displayTokens(const std::vector<Token> &tokens)
{
    tokenTable->setRowCount(tokens.size());
    for (size_t i = 0; i < tokens.size(); i++)
    {
        tokenTable->setItem(i, 0, new QTableWidgetItem(QString::number(i + 1)));
        tokenTable->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(tokens[i].getLexeme())));
        tokenTable->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(Token::tokenTypeToString(tokens[i].getType()))));
        tokenTable->setItem(i, 3, new QTableWidgetItem(QString::number(tokens[i].getLine())));
        tokenTable->setItem(i, 4, new QTableWidgetItem(QString::number(tokens[i].getColumn())));
    }
    tokenTable->resizeColumnsToContents();
}

void MainWindow::displayErrors()
{
    const auto &errors = errorManager->getErrors();
    errorTable->setRowCount(errors.size());
    for (size_t i = 0; i < errors.size(); i++)
    {
        errorTable->setItem(i, 0, new QTableWidgetItem(QString::number(errors[i].id)));
        errorTable->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(errors[i].lexeme)));
        errorTable->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(errors[i].errorType)));
        errorTable->setItem(i, 3, new QTableWidgetItem(QString::fromStdString(errors[i].description)));
        errorTable->setItem(i, 4, new QTableWidgetItem(QString::number(errors[i].line)));
        errorTable->setItem(i, 5, new QTableWidgetItem(QString::number(errors[i].column)));
    }
    errorTable->resizeColumnsToContents();
}

void MainWindow::clearTables()
{
    tokenTable->setRowCount(0);
    errorTable->setRowCount(0);
}

void MainWindow::updateStatus(const QString &message, bool isError)
{
    statusLabel->setText(message);
    statusLabel->setStyleSheet(isError ? "color: red;" : "color: green;");
}

void MainWindow::onClear()
{
    sourceCodeEditor->clear();
    clearTables();
    reportGen->clear();
    updateStatus("Todo limpiado. Listo para nuevo analisis.");
}

void MainWindow::openReportInBrowser(const QString &reportFile)
{
    QUrl url = QUrl::fromLocalFile(reportFile);
    if (!QDesktopServices::openUrl(url))
    {
        QMessageBox::warning(this, "Error", "No se pudo abrir el reporte.");
    }
}

void MainWindow::onViewReport1() { openReportInBrowser("./reports/reporte1_pacientes.html"); }
void MainWindow::onViewReport2() { openReportInBrowser("./reports/reporte2_medicos.html"); }
void MainWindow::onViewReport3() { openReportInBrowser("./reports/reporte3_citas.html"); }
bool MainWindow::isValidBloodType(const std::string &bloodType)
{
    std::vector<std::string> validTypes = {"A+", "A-", "B+", "B-", "O+", "O-", "AB+", "AB-"};
    for (const auto &type : validTypes)
    {
        if (bloodType == type)
            return true;
    }
    return false;
}
void MainWindow::onViewReport4() { openReportInBrowser("./reports/reporte4_estadisticas.html"); }
