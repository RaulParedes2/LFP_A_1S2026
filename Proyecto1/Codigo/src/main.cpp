#include <iostream>
#include <string>
#include <fstream>
#include "../include/core/Token.h"
#include "../include/core/ErrorManager.h"
#include "../include/core/LexicalAnalyzer.h"
#include "../include/reports/ReportGenerator.h"

using namespace std;

int main()
{
    cout << "=== MedLexer v1.0 ===" << endl;
    cout << "Iniciando sistema..." << endl;
    cout << endl;

    // Crear manejador de errores
    ErrorManager errorMgr;

    // Crear analizador léxico
    LexicalAnalyzer lexer(&errorMgr);

    // Código de prueba
    string testCode = R"(
HOSPITAL {
    PACIENTES {
        paciente: "Ana Lopez" [edad: 34, tipo_sangre: "O+"]
        paciente: "Carlos Mendoza" [edad: 62, tipo_sangre: "O-"]
    }
    MEDICOS {
        medico: "Dr. Roberto Juarez" [especialidad: CARDIOLOGIA, codigo: "MED-001"]
        medico: "Dra. Ana Solis" [especialidad: NEUROLOGIA, codigo: "MED-002"]
    }
    CITAS {
        cita: "Ana Lopez" con "Dr. Roberto Juarez" [fecha: 2025-04-10, hora: 09:00]
        cita: "Carlos Mendoza" con "Dra. Ana Solis" [fecha: 2025-04-10, hora: 11:30]
    }
    DIAGNOSTICOS {
        diagnostico: "Ana Lopez" [condicion: "Hipertension Arterial", medicamento: "Losartan 50mg", dosis: DIARIA]
        diagnostico: "Carlos Mendoza" [condicion: "Diabetes Tipo 2", medicamento: "Metformina 850mg", dosis: CADA_12_HORAS]
    }
}
)";

    // Cargar código fuente
    lexer.setSourceCode(testCode);

    // Analizar
    cout << "Analizando codigo fuente..." << endl;
    vector<Token> tokens = lexer.analyze();

    // Mostrar resumen
    cout << endl;
    cout << "=== RESUMEN ANALISIS ===" << endl;
    cout << "Total tokens: " << tokens.size() << endl;
    cout << "Total errores: " << errorMgr.getErrorCount() << endl;

    if (errorMgr.getErrorCount() == 0)
    {
        cout << "Analisis lexico completado sin errores!" << endl;
    }

    // Procesar tokens para generar reportes
    cout << endl;
    cout << "Procesando datos para reportes..." << endl;

    ReportGenerator reportGen;

    // Variables para seguimiento
    vector<Patient> tempPatients;
    vector<Doctor> tempDoctors;
    vector<Appointment> tempAppointments;

    Patient currentPatient;
    Doctor currentDoctor;
    Appointment currentAppointment;

    bool inPatients = false;
    bool inDoctors = false;
    bool inAppointments = false;
    bool inDiagnostics = false;
    bool inPatientBlock = false;
    bool inDoctorBlock = false;
    bool inAppointmentBlock = false;
    bool inDiagnosticBlock = false;

    string currentPatientName;
    string currentDoctorName;

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
            cout << "  Procesando seccion PACIENTES..." << endl;
        }
        else if (t.getType() == TokenType::MEDICOS)
        {
            inPatients = false;
            inDoctors = true;
            inAppointments = false;
            inDiagnostics = false;
            cout << "  Procesando seccion MEDICOS..." << endl;
        }
        else if (t.getType() == TokenType::CITAS)
        {
            inPatients = false;
            inDoctors = false;
            inAppointments = true;
            inDiagnostics = false;
            cout << "  Procesando seccion CITAS..." << endl;
        }
        else if (t.getType() == TokenType::DIAGNOSTICOS)
        {
            inPatients = false;
            inDoctors = false;
            inAppointments = false;
            inDiagnostics = true;
            cout << "  Procesando seccion DIAGNOSTICOS..." << endl;
        }

        // Procesar pacientes
        if (inPatients && t.getType() == TokenType::PACIENTE)
        {
            currentPatient = Patient();
            inPatientBlock = true;
            currentPatientName = "";

            // Buscar el nombre del paciente
            for (size_t j = i + 1; j < tokens.size() && j < i + 5; j++)
            {
                if (tokens[j].getType() == TokenType::STRING)
                {
                    currentPatient.name = tokens[j].getLexeme();
                    currentPatientName = tokens[j].getLexeme();
                    cout << "    Paciente encontrado: " << currentPatient.name << endl;
                    break;
                }
            }
        }

        // Procesar médicos
        if (inDoctors && t.getType() == TokenType::MEDICO)
        {
            currentDoctor = Doctor();
            inDoctorBlock = true;
            currentDoctorName = "";

            for (size_t j = i + 1; j < tokens.size() && j < i + 5; j++)
            {
                if (tokens[j].getType() == TokenType::STRING)
                {
                    currentDoctor.name = tokens[j].getLexeme();
                    currentDoctorName = tokens[j].getLexeme();
                    cout << "    Medico encontrado: " << currentDoctor.name << endl;
                    break;
                }
            }
        }

        // Procesar citas
        if (inAppointments && t.getType() == TokenType::CITA)
        {
            currentAppointment = Appointment();
            inAppointmentBlock = true;
        }

        // Procesar diagnósticos
        if (inDiagnostics && t.getType() == TokenType::DIAGNOSTICO)
        {
            currentPatient = Patient();
            inDiagnosticBlock = true;
            currentPatientName = "";

            for (size_t j = i + 1; j < tokens.size() && j < i + 5; j++)
            {
                if (tokens[j].getType() == TokenType::STRING)
                {
                    currentPatient.name = tokens[j].getLexeme();
                    currentPatientName = tokens[j].getLexeme();
                    cout << "    Diagnostico para: " << currentPatient.name << endl;
                    break;
                }
            }
        }

        // Procesar atributos
        if (t.getType() == TokenType::ATRIBUTO)
        {
            string attr = t.getLexeme();
            if (i + 2 < tokens.size() && tokens[i + 1].getType() == TokenType::COLON)
            {

                // Atributos de paciente
                if (inPatientBlock && attr == "edad" && tokens[i + 2].getType() == TokenType::INTEGER)
                {
                    currentPatient.age = stoi(tokens[i + 2].getLexeme());
                    cout << "      Edad: " << currentPatient.age << endl;
                }
                else if (inPatientBlock && attr == "tipo_sangre" && tokens[i + 2].getType() == TokenType::STRING)
                {
                    currentPatient.bloodType = tokens[i + 2].getLexeme();
                    cout << "      Tipo sangre: " << currentPatient.bloodType << endl;
                    // Guardar paciente
                    if (!currentPatient.name.empty())
                    {
                        tempPatients.push_back(currentPatient);
                        cout << "    Paciente guardado: " << currentPatient.name << endl;
                        inPatientBlock = false;
                    }
                }

                // Atributos de médico
                else if (inDoctorBlock && attr == "especialidad" &&
                         (tokens[i + 2].getType() == TokenType::ESPECIALIDAD ||
                          tokens[i + 2].getType() == TokenType::STRING))
                {
                    currentDoctor.specialty = tokens[i + 2].getLexeme();
                    cout << "      Especialidad: " << currentDoctor.specialty << endl;
                }
                else if (inDoctorBlock && attr == "codigo" && tokens[i + 2].getType() == TokenType::STRING)
                {
                    currentDoctor.code = tokens[i + 2].getLexeme();
                    cout << "      Codigo: " << currentDoctor.code << endl;
                    // Guardar médico
                    if (!currentDoctor.name.empty())
                    {
                        tempDoctors.push_back(currentDoctor);
                        cout << "    Medico guardado: " << currentDoctor.name << endl;
                        inDoctorBlock = false;
                    }
                }

                // Atributos de cita
                else if (inAppointmentBlock && attr == "fecha" && tokens[i + 2].getType() == TokenType::DATE)
                {
                    currentAppointment.date = tokens[i + 2].getLexeme();
                    cout << "      Fecha: " << currentAppointment.date << endl;
                }
                else if (inAppointmentBlock && attr == "hora" && tokens[i + 2].getType() == TokenType::TIME)
                {
                    currentAppointment.time = tokens[i + 2].getLexeme();
                    cout << "      Hora: " << currentAppointment.time << endl;
                }

                // Atributos de diagnóstico
                else if (inDiagnosticBlock && attr == "condicion" && tokens[i + 2].getType() == TokenType::STRING)
                {
                    currentPatient.diagnosis = tokens[i + 2].getLexeme();
                    currentPatient.hasDiagnosis = true;
                    cout << "      Condicion: " << currentPatient.diagnosis << endl;
                }
                else if (inDiagnosticBlock && attr == "medicamento" && tokens[i + 2].getType() == TokenType::STRING)
                {
                    currentPatient.medication = tokens[i + 2].getLexeme();
                    cout << "      Medicamento: " << currentPatient.medication << endl;
                }
                else if (inDiagnosticBlock && attr == "dosis" &&
                         (tokens[i + 2].getType() == TokenType::DOSIS ||
                          tokens[i + 2].getType() == TokenType::STRING))
                {
                    currentPatient.dosis = tokens[i + 2].getLexeme();
                    cout << "      Dosis: " << currentPatient.dosis << endl;
                    // Actualizar diagnóstico del paciente existente
                    bool found = false;
                    for (auto &p : tempPatients)
                    {
                        if (p.name == currentPatient.name)
                        {
                            p.hasDiagnosis = true;
                            p.diagnosis = currentPatient.diagnosis;
                            p.medication = currentPatient.medication;
                            p.dosis = currentPatient.dosis;
                            found = true;
                            cout << "    Diagnostico actualizado para: " << currentPatient.name << endl;
                            break;
                        }
                    }
                    if (!found)
                    {
                        tempPatients.push_back(currentPatient);
                        cout << "    Nuevo paciente con diagnostico: " << currentPatient.name << endl;
                    }
                    inDiagnosticBlock = false;
                }
            }
        }

        // Procesar la palabra "con" para citas
        if (inAppointmentBlock && t.getType() == TokenType::CON)
        {
            // Buscar el nombre del paciente
            for (int j = i - 1; j >= 0 && j > (int)i - 5; j--)
            {
                if (tokens[j].getType() == TokenType::STRING)
                {
                    if (currentAppointment.patient.empty())
                    {
                        currentAppointment.patient = tokens[j].getLexeme();
                    }
                }
            }

            // Buscar el nombre del médico después de "con"
            for (size_t j = i + 1; j < tokens.size() && j < i + 5; j++)
            {
                if (tokens[j].getType() == TokenType::STRING)
                {
                    currentAppointment.doctor = tokens[j].getLexeme();
                    // Buscar especialidad del médico
                    for (const auto &d : tempDoctors)
                    {
                        if (d.name == currentAppointment.doctor)
                        {
                            currentAppointment.specialty = d.specialty;
                            break;
                        }
                    }
                    break;
                }
            }
        }

        // Cuando se cierra un bloque de cita, guardar
        if (inAppointmentBlock && t.getType() == TokenType::RBRACKET &&
            !currentAppointment.patient.empty() && !currentAppointment.doctor.empty())
        {
            tempAppointments.push_back(currentAppointment);
            cout << "    Cita guardada: " << currentAppointment.patient << " -> "
                 << currentAppointment.doctor << " (" << currentAppointment.date << " "
                 << currentAppointment.time << ")" << endl;
            inAppointmentBlock = false;
        }
    }

    // Agregar todos los datos al ReportGenerator
    for (const auto &p : tempPatients)
    {
        reportGen.addPatient(p);
    }
    for (const auto &d : tempDoctors)
    {
        reportGen.addDoctor(d);
    }
    for (const auto &a : tempAppointments)
    {
        reportGen.addAppointment(a);
    }

    // Actualizar conteo de citas por médico
    vector<Doctor> updatedDoctors;
    for (auto &d : tempDoctors)
    {
        Doctor updated = d;
        updated.appointments = 0;
        updated.patients.clear();

        // Contar citas y pacientes para este médico
        for (const auto &a : tempAppointments)
        {
            if (a.doctor == updated.name)
            {
                updated.appointments++;
                // Agregar paciente si no está ya en la lista
                bool found = false;
                for (const auto &p : updated.patients)
                {
                    if (p == a.patient)
                    {
                        found = true;
                        break;
                    }
                }
                if (!found)
                {
                    updated.patients.push_back(a.patient);
                }
            }
        }
        updatedDoctors.push_back(updated);
        cout << "  Medico " << updated.name << ": " << updated.appointments << " citas, "
             << updated.getUniquePatients() << " pacientes unicos" << endl;
    }

    // Limpiar y volver a agregar con los datos actualizados
    reportGen.clear();
    for (const auto &p : tempPatients)
    {
        reportGen.addPatient(p);
    }
    for (const auto &d : updatedDoctors)
    {
        reportGen.addDoctor(d);
    }
    for (const auto &a : tempAppointments)
    {
        reportGen.addAppointment(a);
    }

    // Mostrar resumen de datos extraídos
    cout << endl;
    cout << "=== DATOS EXTRAIDOS ===" << endl;
    cout << "Pacientes: " << tempPatients.size() << endl;
    cout << "Medicos: " << tempDoctors.size() << endl;
    cout << "Citas: " << tempAppointments.size() << endl;

    // Mostrar médicos con sus citas
    for (const auto &d : updatedDoctors)
    {
        cout << "  - " << d.name << " (" << d.specialty << ", " << d.code
             << ") - " << d.appointments << " citas, "
             << d.getUniquePatients() << " pacientes" << endl;
    }

    // Mostrar resumen de datos extraídos
    cout << endl;
    cout << "=== DATOS EXTRAIDOS ===" << endl;
    cout << "Pacientes: " << tempPatients.size() << endl;
    cout << "Medicos: " << tempDoctors.size() << endl;
    cout << "Citas: " << tempAppointments.size() << endl;

    for (const auto &p : tempPatients)
    {
        cout << "  - " << p.name << " (" << p.age << " años, " << p.bloodType << ")";
        if (p.hasDiagnosis)
        {
            cout << " [Diagnostico: " << p.diagnosis << "]";
        }
        cout << endl;
    }

    for (const auto &d : tempDoctors)
    {
        cout << "  - " << d.name << " (" << d.specialty << ", " << d.code << ")" << endl;
    }

    for (const auto &a : tempAppointments)
    {
        cout << "  - Cita: " << a.patient << " con " << a.doctor << " (" << a.date << " " << a.time << ")" << endl;
    }

    // Generar reportes
    cout << endl;
    cout << "Generando reportes..." << endl;

    if (reportGen.generateAllReports("./"))
    {
        cout << "Reportes HTML generados exitosamente!" << endl;
        cout << "  - reporte1_pacientes.html" << endl;
        cout << "  - reporte2_medicos.html" << endl;
        cout << "  - reporte3_citas.html" << endl;
        cout << "  - reporte4_estadisticas.html" << endl;
    }
    else
    {
        cout << "Error al generar reportes" << endl;
    }

    // Generar diagrama Graphviz
    if (reportGen.generateGraphviz("hospital.dot"))
    {
        cout << "Diagrama Graphviz generado: hospital.dot" << endl;
        cout << "  Para visualizar: dot -Tpng hospital.dot -o hospital.png" << endl;
    }
    else
    {
        cout << "Error al generar diagrama Graphviz" << endl;
    }

    cout << endl;
    cout << "=== PROCESO COMPLETADO ===" << endl;

    return 0;
}