#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include <QTableWidget>
#include <QPushButton>
#include <QLabel>
#include <QSplitter>
#include <QTabWidget>
#include "../core/LexicalAnalyzer.h"
#include "../core/ErrorManager.h"
#include "../reports/ReportGenerator.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onLoadFile();
    void onAnalyze();
    void onClear();
    void onViewReport1();
    void onViewReport2();
    void onViewReport3();
    void onViewReport4();

private:
    // Componentes de la GUI
    QTextEdit *sourceCodeEditor;
    QTableWidget *tokenTable;
    QTableWidget *errorTable;
    QTabWidget *tabWidget;
    QPushButton *loadButton;
    QPushButton *analyzeButton;
    QPushButton *clearButton;
    QPushButton *report1Button;
    QPushButton *report2Button;
    QPushButton *report3Button;
    QPushButton *report4Button;
    QLabel *statusLabel;
    
    // Componentes lógicos
    LexicalAnalyzer *lexer;
    ErrorManager *errorManager;
    ReportGenerator *reportGen;
    
    // Archivo actual
    QString currentFile;
    
    // Métodos auxiliares
    void setupUI();
    void setupConnections();
    void displayTokens(const std::vector<Token>& tokens);
    void displayErrors();
    void clearTables();
    void updateStatus(const QString& message, bool isError = false);
    void openReportInBrowser(const QString& reportFile);
};

#endif