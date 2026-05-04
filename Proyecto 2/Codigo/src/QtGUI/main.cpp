#include <QApplication>
#include <QMessageBox>
#include <iostream>
#include "MainWindow.h"

// Manejador de excepciones global
void globalExceptionHandler()
{
    try {
        throw;
    } catch (const std::exception& e) {
        QMessageBox::critical(nullptr, "Error Fatal", 
            QString("Excepción no manejada:\n%1").arg(e.what()));
    } catch (...) {
        QMessageBox::critical(nullptr, "Error Fatal", 
            "Excepción desconocida no manejada.");
    }
}

int main(int argc, char *argv[])
{
    // Instalar manejador de excepciones
    std::set_terminate(globalExceptionHandler);
    
    QApplication app(argc, argv);
    
    MainWindow window;
    window.show();
    
    return app.exec();
}