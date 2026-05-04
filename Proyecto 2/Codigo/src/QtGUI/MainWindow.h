#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include <QTableWidget>
#include <QPushButton>
#include <QSplitter>
#include <QTabWidget>
#include <QStatusBar>
#include <QLabel>
#include "../Lexer/Token.h"
#include "../Lexer/LexicalAnalyzer.h"
#include "../Error/ErrorManager.h"
#include "../Parser/SyntaxAnalyzer.h"
#include "../Reports/ReportGenerator.h"
#include <memory>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onLoadFile();
    void onAnalyze();
    void onGenerateReports();
    void onSaveDot();
    void onViewGraph();
    void onClearData();  // Nuevo slot para limpiar
    void onExit();

private:
    // Widgets
    QTextEdit* textEditor;
    QTableWidget* tokenTable;
    QTableWidget* errorTable;
    QTextEdit* dotDisplay;
    QTabWidget* tabWidget;
    QPushButton* btnLoad;
    QPushButton* btnAnalyze;
    QPushButton* btnReports;
    QPushButton* btnSaveDot;
    QPushButton* btnViewGraph;
    QPushButton* btnClear;  // Nuevo botón
    QStatusBar* statusBar;
    
    // Datos
    QString currentFilename;
    std::vector<TaskScript::Token> tokens;
    std::unique_ptr<TaskScript::ASTNode> ast;
    TaskScript::ErrorManager lexErrors;
    TaskScript::ErrorManager syntaxErrors;
    QString dotCode;
    
    // Metodos auxiliares
    void setupUI();
    void updateTokenTable();
    void updateErrorTable();
    void updateDotDisplay();
    void clearTables();
};

#endif // MAINWINDOW_H