#include "../../include/reports/ReportGenerator.h"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <map>

using namespace std;

// Implementación de métodos auxiliares de Doctor
int Doctor::getUniquePatients() const
{
    vector<string> unique;
    for (const auto &p : patients)
    {
        if (find(unique.begin(), unique.end(), p) == unique.end())
        {
            unique.push_back(p);
        }
    }
    return unique.size();
}

// Constructor
ReportGenerator::ReportGenerator() {}

ReportGenerator::~ReportGenerator() {}

void ReportGenerator::clear()
{
    patients.clear();
    doctors.clear();
    appointments.clear();
    stats = HospitalStats();
}

void ReportGenerator::addPatient(const Patient &patient)
{
    patients.push_back(patient);
}

void ReportGenerator::addDoctor(const Doctor &doctor)
{
    doctors.push_back(doctor);
}

void ReportGenerator::addAppointment(const Appointment &appointment)
{
    appointments.push_back(appointment);
}

void ReportGenerator::detectAppointmentConflicts()
{
    stats.conflictsDetected = 0;

    for (auto &app : appointments)
    {
        app.hasConflict = false;
        // Verificar conflictos con otras citas del mismo médico
        for (const auto &other : appointments)
        {
            if (&app != &other &&
                app.doctor == other.doctor &&
                app.date == other.date &&
                app.time == other.time)
            {
                app.hasConflict = true;
                stats.conflictsDetected++;
                break;
            }
        }
    }
    // Cada conflicto se cuenta dos veces, dividir entre 2
    stats.conflictsDetected /= 2;
}

void ReportGenerator::updateMedicationFrequency(const string &medication)
{
    stats.medicationFrequency[medication]++;
    if (stats.medicationFrequency[medication] > stats.mostPrescribedCount)
    {
        stats.mostPrescribedCount = stats.medicationFrequency[medication];
        stats.mostPrescribedMed = medication;
    }
}

void ReportGenerator::calculateAverageAge()
{
    if (patients.empty())
    {
        stats.averageAge = 0;
        return;
    }

    double sum = 0;
    for (const auto &p : patients)
    {
        sum += p.age;
    }
    stats.averageAge = sum / patients.size();
}

void ReportGenerator::findBusiestSpecialty()
{
    map<string, pair<int, string>> specialtyStats; // specialty -> (appointments, doctor)

    for (const auto &app : appointments)
    {
        specialtyStats[app.specialty].first++;
        specialtyStats[app.specialty].second = app.doctor;
    }

    for (const auto &stat : specialtyStats)
    {
        if (stat.second.first > stats.busiestDoctorAppointments)
        {
            stats.busiestSpecialty = stat.first;
            stats.busiestDoctor = stat.second.second;
            stats.busiestDoctorAppointments = stat.second.first;
        }
    }
}

void ReportGenerator::calculateStatistics()
{
    stats.totalPatients = patients.size();
    stats.totalDoctors = doctors.size();
    stats.totalAppointments = appointments.size();

    stats.patientsWithDiagnosis = 0;
    for (const auto &p : patients)
    {
        if (p.hasDiagnosis)
        {
            stats.patientsWithDiagnosis++;
        }
        if (!p.medication.empty())
        {
            updateMedicationFrequency(p.medication);
        }
    }

    detectAppointmentConflicts();
    calculateAverageAge();
    findBusiestSpecialty();

    // Nombre del hospital (por defecto)
    stats.name = "Hospital General San Carlos";
}

string ReportGenerator::generateCSS()
{
    return R"(
    <style>
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }
        body {
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            min-height: 100vh;
            padding: 20px;
        }
        .container {
            max-width: 1200px;
            margin: 0 auto;
            background: white;
            border-radius: 15px;
            box-shadow: 0 20px 60px rgba(0,0,0,0.3);
            overflow: hidden;
        }
        .header {
            background: linear-gradient(135deg, #2c3e50 0%, #3498db 100%);
            color: white;
            padding: 30px;
            text-align: center;
        }
        .header h1 {
            font-size: 2.5em;
            margin-bottom: 10px;
        }
        .header p {
            font-size: 1.1em;
            opacity: 0.9;
        }
        .content {
            padding: 30px;
        }
        table {
            width: 100%;
            border-collapse: collapse;
            margin: 20px 0;
            box-shadow: 0 1px 3px rgba(0,0,0,0.1);
        }
        th {
            background: #3498db;
            color: white;
            padding: 12px;
            text-align: left;
            font-weight: 600;
        }
        td {
            padding: 10px 12px;
            border-bottom: 1px solid #e0e0e0;
        }
        tr:hover {
            background: #f5f5f5;
        }
        .status-active {
            background: #d4edda;
            color: #155724;
            padding: 5px 10px;
            border-radius: 5px;
            font-weight: bold;
            display: inline-block;
        }
        .status-no-diagnosis {
            background: #fff3cd;
            color: #856404;
            padding: 5px 10px;
            border-radius: 5px;
            font-weight: bold;
            display: inline-block;
        }
        .status-critical {
            background: #f8d7da;
            color: #721c24;
            padding: 5px 10px;
            border-radius: 5px;
            font-weight: bold;
            display: inline-block;
        }
        .conflict-row {
            background: #f8d7da !important;
        }
        .kpi-grid {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(300px, 1fr));
            gap: 20px;
            margin: 20px 0;
        }
        .kpi-card {
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            color: white;
            padding: 20px;
            border-radius: 10px;
            text-align: center;
        }
        .kpi-card h3 {
            font-size: 0.9em;
            opacity: 0.9;
            margin-bottom: 10px;
        }
        .kpi-card .value {
            font-size: 2em;
            font-weight: bold;
        }
        .progress-bar {
            background: #e0e0e0;
            height: 30px;
            border-radius: 15px;
            overflow: hidden;
            margin: 10px 0;
        }
        .progress-fill {
            background: #3498db;
            height: 100%;
            display: flex;
            align-items: center;
            justify-content: flex-end;
            padding-right: 10px;
            color: white;
            font-weight: bold;
        }
        .warning {
            background: #e74c3c !important;
        }
        .footer {
            background: #f8f9fa;
            padding: 20px;
            text-align: center;
            color: #666;
            font-size: 0.9em;
        }
    </style>
    )";
}

string ReportGenerator::generateReport1()
{
    stringstream html;
    html << "<!DOCTYPE html>\n";
    html << "<html>\n<head>\n";
    html << "<meta charset='UTF-8'>\n";
    html << "<title>Reporte 1 - Historial de Pacientes</title>\n";
    html << generateCSS();
    html << "</head>\n<body>\n";
    html << "<div class='container'>\n";
    html << "<div class='header'>\n";
    html << "<h1>Historial de Pacientes</h1>\n";
    html << "<p>Estado de salud y diagnósticos activos</p>\n";
    html << "</div>\n";
    html << "<div class='content'>\n";

    html << "<table>\n";
    html << "<thead>\n<tr>\n";
    html << "<th>Paciente</th><th>Edad</th><th>Sangre</th>";
    html << "<th>Diagnóstico activo</th><th>Medicamento / Dosis</th><th>Estado</th>\n";
    html << "</tr>\n</thead>\n<tbody>\n";

    for (const auto &p : patients)
    {
        html << "<tr>\n";
        html << "<td>" << p.name << "</td>\n";
        html << "<td>" << p.age << "</td>\n";
        html << "<td>" << p.bloodType << "</td>\n";

        if (p.hasDiagnosis)
        {
            html << "<td>" << p.diagnosis << "</td>\n";
            html << "<td>" << p.medication << " / " << p.dosis << "</td>\n";
            html << "<td><span class='status-active'>ACTIVO</span></td>\n";
        }
        else
        {
            html << "<td>—</td>\n";
            html << "<td>—</td>\n";
            html << "<td><span class='status-no-diagnosis'>SIN DIAG.</span></td>\n";
        }
        html << "</tr>\n";
    }

    html << "</tbody>\n</table>\n";
    html << "</div>\n<div class='footer'>\n";
    html << "<p>Reporte generado por MedLexer - Sistema de Gestión Hospitalaria</p>\n";
    html << "</div>\n</div>\n</body>\n</html>";

    return html.str();
}

string ReportGenerator::generateReport2()
{
    stringstream html;
    html << "<!DOCTYPE html>\n";
    html << "<html>\n<head>\n";
    html << "<meta charset='UTF-8'>\n";
    html << "<title>Reporte 2 - Carga de Médicos por Especialidad</title>\n";
    html << generateCSS();
    html << "</head>\n<body>\n";
    html << "<div class='container'>\n";
    html << "<div class='header'>\n";
    html << "<h1>Carga de Médicos por Especialidad</h1>\n";
    html << "<p>Distribución de citas y carga de trabajo</p>\n";
    html << "</div>\n";
    html << "<div class='content'>\n";

    html << "<table>\n";
    html << "<thead>\n<tr>\n";
    html << "<th>Médico</th><th>Código</th><th>Especialidad</th>";
    html << "<th>Citas Prog.</th><th>Pacientes</th><th>Nivel de Carga</th>\n";
    html << "</tr>\n</thead>\n<tbody>\n";

    for (const auto &d : doctors)
    {
        int citas = d.appointments;
        int pacientes = d.getUniquePatients();

        string nivel;
        string colorClass;
        if (citas <= 3)
        {
            nivel = "BAJA";
            colorClass = "status-active";
        }
        else if (citas <= 6)
        {
            nivel = "NORMAL";
            colorClass = "status-active";
        }
        else if (citas <= 8)
        {
            nivel = "ALTA";
            colorClass = "status-critical";
        }
        else
        {
            nivel = "SATURADA";
            colorClass = "status-critical";
        }

        html << "<tr>\n";
        html << "<td>" << d.name << "</td>\n";
        html << "<td>" << d.code << "</td>\n";
        html << "<td>" << d.specialty << "</td>\n";
        html << "<td>" << citas << "</td>\n";
        html << "<td>" << pacientes << "</td>\n";
        html << "<td><span class='" << colorClass << "'>" << nivel << "</span></td>\n";
        html << "</tr>\n";
    }

    html << "</tbody>\n</table>\n";
    html << "</div>\n<div class='footer'>\n";
    html << "<p>Reporte generado por MedLexer - Sistema de Gestión Hospitalaria</p>\n";
    html << "</div>\n</div>\n</body>\n</html>";

    return html.str();
}

string ReportGenerator::generateReport3()
{
    stringstream html;
    html << "<!DOCTYPE html>\n";
    html << "<html>\n<head>\n";
    html << "<meta charset='UTF-8'>\n";
    html << "<title>Reporte 3 - Agenda de Citas con Detección de Conflictos</title>\n";
    html << generateCSS();
    html << "</head>\n<body>\n";
    html << "<div class='container'>\n";
    html << "<div class='header'>\n";
    html << "<h1> Agenda de Citas</h1>\n";
    html << "<p>Detección automática de conflictos de horario</p>\n";
    html << "</div>\n";
    html << "<div class='content'>\n";

    html << "<table>\n";
    html << "<thead>\n<tr>\n";
    html << "<th>Fecha</th><th>Hora</th><th>Paciente</th>";
    html << "<th>Médico</th><th>Especialidad</th><th>Estado</th>\n";
    html << "</tr>\n</thead>\n<tbody>\n";

    for (const auto &a : appointments)
    {
        string rowClass = a.hasConflict ? "class='conflict-row'" : "";
        html << "<tr " << rowClass << ">\n";
        html << "<td>" << a.date << "</td>\n";
        html << "<td>" << a.time << "</td>\n";
        html << "<td>" << a.patient << "</td>\n";
        html << "<td>" << a.doctor << "</td>\n";
        html << "<td>" << a.specialty << "</td>\n";

        if (a.hasConflict)
        {
            html << "<td><span class='status-critical'>⚠ CONFLICTO</span></td>\n";
        }
        else
        {
            html << "<td><span class='status-active'>✓ CONFIRMADA</span></td>\n";
        }
        html << "</tr>\n";
    }

    html << "</tbody>\n</table>\n";
    html << "</div>\n<div class='footer'>\n";
    html << "<p> Las filas en rojo indican conflictos de horario (mismo médico, misma fecha y hora)</p>\n";
    html << "</div>\n</div>\n</body>\n</html>";

    return html.str();
}

string ReportGenerator::generateReport4()
{
    stringstream html;

    double percentageWithDiagnosis = stats.totalPatients > 0 ? (stats.patientsWithDiagnosis * 100.0 / stats.totalPatients) : 0;

    html << "<!DOCTYPE html>\n";
    html << "<html>\n<head>\n";
    html << "<meta charset='UTF-8'>\n";
    html << "<title>Reporte 4 - Estadístico General del Hospital</title>\n";
    html << generateCSS();
    html << "</head>\n<body>\n";
    html << "<div class='container'>\n";
    html << "<div class='header'>\n";
    html << "<h1>Estadístico General del Hospital</h1>\n";
    html << "<p>Indicadores clave y distribución de carga</p>\n";
    html << "</div>\n";
    html << "<div class='content'>\n";

    // KPIs
    html << "<div class='kpi-grid'>\n";
    html << "<div class='kpi-card'><h3>Nombre del Hospital</h3><div class='value'>" << stats.name << "</div></div>\n";
    html << "<div class='kpi-card'><h3>Total Pacientes</h3><div class='value'>" << stats.totalPatients << "</div></div>\n";
    html << "<div class='kpi-card'><h3>Total Médicos</h3><div class='value'>" << stats.totalDoctors << "</div></div>\n";
    html << "<div class='kpi-card'><h3>Total Citas</h3><div class='value'>" << stats.totalAppointments << "</div></div>\n";
    html << "<div class='kpi-card'><h3>Citas con Conflicto</h3><div class='value'>" << stats.conflictsDetected << " ⚠</div></div>\n";
    html << "<div class='kpi-card'><h3>Pacientes con Diagnóstico</h3><div class='value'>" << stats.patientsWithDiagnosis << " de " << stats.totalPatients << " (" << fixed << setprecision(1) << percentageWithDiagnosis << "%)</div></div>\n";
    html << "<div class='kpi-card'><h3>Medicamento más prescrito</h3><div class='value'>" << stats.mostPrescribedMed << "<br><small>(" << stats.mostPrescribedCount << " pacientes)</small></div></div>\n";
    html << "<div class='kpi-card'><h3>Especialidad con mayor carga</h3><div class='value'>" << stats.busiestSpecialty << "<br><small>" << stats.busiestDoctor << " (" << stats.busiestDoctorAppointments << " citas)</small></div></div>\n";
    html << "<div class='kpi-card'><h3>Promedio de edad</h3><div class='value'>" << fixed << setprecision(1) << stats.averageAge << " años</div></div>\n";
    html << "</div>\n";

    // Distribución por especialidad
    html << "<h2 style='margin-top: 30px;'>Distribución de carga por especialidad</h2>\n";
    html << "<table>\n";
    html << "<thead>\n<tr>\n";
    html << "<th>Especialidad</th><th>Médicos</th><th>Citas</th><th>Pacientes</th><th>Barra de ocupación</th>\n";
    html << "</tr>\n</thead>\n<tbody>\n";

    map<string, pair<int, int>> specialtyStats; // specialty -> (citas, pacientes)
    map<string, int> specialtyDoctors;

    for (const auto &d : doctors)
    {
        specialtyDoctors[d.specialty]++;
    }

    for (const auto &a : appointments)
    {
        specialtyStats[a.specialty].first++;
    }

    for (const auto &spec : specialtyStats)
    {
        double percentage = stats.totalAppointments > 0 ? (spec.second.first * 100.0 / stats.totalAppointments) : 0;

        string barColor = percentage > 80 ? "warning" : "";
        string fillClass = barColor.empty() ? "progress-fill" : "progress-fill warning";

        html << "<tr>\n";
        html << "<td>" << spec.first << "</td>\n";
        html << "<td>" << specialtyDoctors[spec.first] << "</td>\n";
        html << "<td>" << spec.second.first << "</td>\n";
        html << "<td>" << spec.second.second << "</td>\n";
        html << "<td>\n";
        html << "<div class='progress-bar'>\n";
        html << "<div class='" << fillClass << "' style='width: " << percentage << "%;'>" << fixed << setprecision(0) << percentage << "%</div>\n";
        html << "</div>\n";
        html << "</td>\n";
        html << "</tr>\n";
    }

    html << "</tbody>\n</table>\n";
    html << "</div>\n<div class='footer'>\n";
    html << "<p>Reporte generado por MedLexer - Sistema de Gestión Hospitalaria</p>\n";
    html << "</div>\n</div>\n</body>\n</html>";

    return html.str();
}

bool ReportGenerator::writeHTMLFile(const string &filename, const string &content)
{
    ofstream file(filename);
    if (!file.is_open())
    {
        return false;
    }
    file << content;
    file.close();
    return true;
}

bool ReportGenerator::generateAllReports(const string &outputDir)
{
    calculateStatistics();

    string dir = outputDir;
    if (!dir.empty() && dir.back() != '/' && dir.back() != '\\')
    {
        dir += "/";
    }

    bool success = true;
    success &= writeHTMLFile(dir + "reporte1_pacientes.html", generateReport1());
    success &= writeHTMLFile(dir + "reporte2_medicos.html", generateReport2());
    success &= writeHTMLFile(dir + "reporte3_citas.html", generateReport3());
    success &= writeHTMLFile(dir + "reporte4_estadisticas.html", generateReport4());

    return success;
}

bool ReportGenerator::generateGraphviz(const string &filename)
{
    stringstream dot;
    dot << "digraph Hospital {\n";
    dot << "  rankdir=TB;\n";
    dot << "  node [shape=box, style=filled, fontname=\"Arial\"];\n\n";

    dot << "  H [label=\"" << stats.name << "\", fillcolor=\"#1A4731\", fontcolor=white, shape=ellipse];\n";
    dot << "  P [label=\"PACIENTES\", fillcolor=\"#2E7D52\", fontcolor=white];\n";
    dot << "  M [label=\"MEDICOS\", fillcolor=\"#2E7D52\", fontcolor=white];\n";
    dot << "  C [label=\"CITAS\", fillcolor=\"#2E7D52\", fontcolor=white];\n";
    dot << "  D [label=\"DIAGNOSTICOS\", fillcolor=\"#2E7D52\", fontcolor=white];\n\n";

    dot << "  H -> P; H -> M; H -> C; H -> D;\n\n";

    // Nodos de pacientes
    for (const auto &p : patients)
    {
        dot << "  p" << &p << " [label=\"" << p.name << "\\n"
            << p.age << " años | " << p.bloodType << "\", fillcolor=\"#D4EDDA\"];\n";
        dot << "  P -> p" << &p << ";\n";
    }
    dot << "\n";

    // Nodos de médicos
    for (const auto &d : doctors)
    {
        dot << "  m" << &d << " [label=\"" << d.name << "\\n"
            << d.code << " | " << d.specialty << "\", fillcolor=\"#D6EAF8\"];\n";
        dot << "  M -> m" << &d << ";\n";
    }
    dot << "\n";

    // Conexiones de citas
    for (const auto &a : appointments)
    {
        // Buscar paciente y médico
        for (const auto &p : patients)
        {
            if (p.name == a.patient)
            {
                for (const auto &d : doctors)
                {
                    if (d.name == a.doctor)
                    {
                        dot << "  p" << &p << " -> m" << &d << " [label=\"" << a.date << " " << a.time << "\", color=\"#E67E22\", style=dashed];\n";
                        break;
                    }
                }
                break;
            }
        }
    }
    dot << "\n";

    // Diagnósticos
    int diagNum = 1;
    for (const auto &p : patients)
    {
        if (p.hasDiagnosis)
        {
            dot << "  d" << diagNum << " [label=\"" << p.diagnosis << "\\n"
                << p.medication << " / " << p.dosis << "\", fillcolor=\"#FDEBD0\"];\n";
            dot << "  D -> d" << diagNum << ";\n";
            dot << "  d" << diagNum << " -> p" << &p << " [label=\"diagnóstico activo\", color=\"#C0392B\"];\n";
            diagNum++;
        }
    }

    dot << "}\n";

    ofstream file(filename);
    if (!file.is_open())
    {
        return false;
    }
    file << dot.str();
    file.close();
    return true;
}