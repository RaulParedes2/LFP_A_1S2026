#ifndef REPORT_GENERATOR_H
#define REPORT_GENERATOR_H

#include <string>
#include <vector>
#include <map>
#include "../core/Token.h"

using namespace std;

// Estructura para almacenar información de pacientes
struct Patient{
    string name;
    int age;
    string bloodType;
    string diagnosis;
    string medication;
    string dosis;
    bool hasDiagnosis;

    Patient() : age(0), hasDiagnosis(false){}
};
// Estructura para almacenar información de médicos
struct Doctor
{
    string name;
    string code;
    string specialty;
    int appointments;
    vector<string> patients;
    
    Doctor() : appointments(0){}
    int getUniquePatients() const;
};


// Estructura para almacenar información de citas

struct Appointment
{
    string date;
    string time;
    string patient;
    string doctor;
    string specialty;
    bool hasConflict;

    Appointment() : hasConflict(false){}
};

// Estructura para estadísticas globales

struct HospitalStats
{
    string name;
    int totalPatients;
    int totalDoctors;
    int totalAppointments;
    int conflictsDetected;
    int patientsWithDiagnosis;
    map<string, int> medicationFrequency;
    string mostPrescribedMed;
    int mostPrescribedCount;
    string busiestSpecialty;
    string busiestDoctor;
    int busiestDoctorAppointments;
    double averageAge;

     HospitalStats() : totalPatients(0), totalDoctors(0), totalAppointments(0),
                      conflictsDetected(0), patientsWithDiagnosis(0),
                      mostPrescribedCount(0), busiestDoctorAppointments(0),
                      averageAge(0.0) {}

};

class ReportGenerator {
    private:
    vector<Patient> patients;
    vector<Doctor> doctors;
    vector<Appointment> appointments;
    HospitalStats stats;
    
    // Métodos de procesamiento de datos
    void calculateStatistics();
    void detectAppointmentConflicts();
    void updateMedicationFrequency(const string& medication);
    void calculateAverageAge();
    void findBusiestSpecialty();

    string generateCSS();
    string generateReport1();
    string generateReport2();
    string generateReport3();
    string generateReport4();

    // Método para escribir archivo
    bool writeHTMLFile(const std::string& filename, const std::string& content);
    
public:
    ReportGenerator();
    ~ReportGenerator();
    
    // Métodos para agregar datos
    void addPatient(const Patient& patient);
    void addDoctor(const Doctor& doctor);
    void addAppointment(const Appointment& appointment);
    
    // Métodos para obtener datos
    const std::vector<Patient>& getPatients() const { return patients; }
    const std::vector<Doctor>& getDoctors() const { return doctors; }
    const std::vector<Appointment>& getAppointments() const { return appointments; }
    
    // Generar todos los reportes
    bool generateAllReports(const std::string& outputDir);
    
    // Generar diagrama Graphviz
    bool generateGraphviz(const std::string& filename);
    
    // Limpiar datos
    void clear();
};

#endif



