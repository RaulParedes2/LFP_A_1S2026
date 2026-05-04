// Definir macros antes de cualquier include para evitar conflictos
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <string>
#include <vector>
#include <map>
#include <cstdlib>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#endif

// Incluir nuestros headers después de los de Windows
#include "Lexer/Token.h"
#include "Lexer/LexicalAnalyzer.h"
#include "Error/ErrorManager.h"
#include "Parser/SyntaxAnalyzer.h"
#include "Reports/ReportGenerator.h"

using namespace TaskScript;

// Función para escapar cadenas JSON
std::string escapeJson(const std::string& str) {
    std::stringstream ss;
    for (char c : str) {
        switch (c) {
            case '"': ss << "\\\""; break;
            case '\\': ss << "\\\\"; break;
            case '\n': ss << "\\n"; break;
            case '\r': ss << "\\r"; break;
            case '\t': ss << "\\t"; break;
            default: ss << c; break;
        }
    }
    return ss.str();
}

// Generar JSON con resultados
std::string generateJsonResult(const std::vector<Token>& tokens, 
                                const ErrorManager& errors,
                                const std::string& dotCode) {
    std::stringstream json;
    json << "{\n";
    json << "  \"success\": true,\n";
    json << "  \"tokens\": [\n";
    
    int count = 1;
    for (const auto& token : tokens) {
        if (token.getType() == TokenType::FIN_DE_ARCHIVO) continue;
        
        json << "    {";
        json << "\"no\": " << count++ << ", ";
        json << "\"lexeme\": \"" << escapeJson(token.getLexeme()) << "\", ";
        json << "\"type\": \"" << token.typeToString() << "\", ";
        json << "\"line\": " << token.getLine() << ", ";
        json << "\"column\": " << token.getColumn();
        json << "}";
        json << "\n";
    }
    
    json << "  ],\n";
    json << "  \"errors\": [\n";
    
    const auto& errorList = errors.getAllErrors();
    for (size_t i = 0; i < errorList.size(); i++) {
        const auto& error = errorList[i];
        json << "    {";
        json << "\"no\": " << error.getId() << ", ";
        json << "\"lexeme\": \"" << escapeJson(error.getLexeme()) << "\", ";
        json << "\"type\": \"" << error.getTypeString() << "\", ";
        json << "\"description\": \"" << escapeJson(error.getDescription()) << "\", ";
        json << "\"line\": " << error.getLine() << ", ";
        json << "\"column\": " << error.getColumn() << ", ";
        json << "\"severity\": \"" << error.getSeverityString() << "\"";
        json << "}";
        if (i < errorList.size() - 1) json << ",";
        json << "\n";
    }
    
    json << "  ],\n";
    json << "  \"dotCode\": \"" << escapeJson(dotCode) << "\"\n";
    json << "}\n";
    
    return json.str();
}

// Analizar código fuente
std::string analyzeCode(const std::string& sourceCode) {
    // Guardar código temporal
    std::ofstream tempFile("temp_analysis.task");
    tempFile << sourceCode;
    tempFile.close();
    
    // Análisis léxico
    ErrorManager lexErrors;
    LexicalAnalyzer lexer(lexErrors);
    
    std::vector<Token> tokens;
    
    if (lexer.loadFile("temp_analysis.task")) {
        Token token = lexer.getNextToken();
        while (token.getType() != TokenType::FIN_DE_ARCHIVO) {
            tokens.push_back(token);
            token = lexer.getNextToken();
        }
    }
    
    // Análisis sintáctico
    ErrorManager syntaxErrors;
    lexer.reset();
    SyntaxAnalyzer parser(lexer, syntaxErrors);
    auto ast = parser.parse();
    
    std::string dotCode;
    if (ast != nullptr) {
        dotCode = parser.generateDotCode();
        
        // Generar reportes HTML
        ReportGenerator reporter;
        reporter.loadData(tokens, syntaxErrors.getAllErrors());
        reporter.loadAST(ast.get());
        reporter.saveAllReports("web/reporte");
    }
    
    // Limpiar temporal
    std::remove("temp_analysis.task");
    
    return generateJsonResult(tokens, syntaxErrors, dotCode);
}

// Servir archivo estático
std::string serveFile(const std::string& path) {
    std::string filepath = "web/" + path;
    if (path == "/" || path.empty()) {
        filepath = "web/index.html";
    }
    
    std::ifstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        return "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\n\r\n<h1>404 - Archivo no encontrado</h1>";
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    // Determinar Content-Type
    std::string contentType = "text/html";
    if (filepath.find(".css") != std::string::npos) contentType = "text/css";
    else if (filepath.find(".js") != std::string::npos) contentType = "application/javascript";
    else if (filepath.find(".png") != std::string::npos) contentType = "image/png";
    
    std::stringstream response;
    response << "HTTP/1.1 200 OK\r\n";
    response << "Content-Type: " << contentType << "\r\n";
    response << "Content-Length: " << content.length() << "\r\n";
    response << "Access-Control-Allow-Origin: *\r\n";
    response << "Connection: close\r\n";
    response << "\r\n";
    response << content;
    
    return response.str();
}

// Procesar petición POST para análisis
std::string handleAnalyze(const std::string& body) {
    // Extraer código del body
    std::string code;
    size_t pos = body.find("code=");
    if (pos != std::string::npos) {
        std::string encoded = body.substr(pos + 5);
        // Decodificar URL
        for (size_t i = 0; i < encoded.length(); i++) {
            if (encoded[i] == '+') {
                code += ' ';
            } else if (encoded[i] == '%' && i + 2 < encoded.length()) {
                char hex[3] = {encoded[i+1], encoded[i+2], 0};
                char c = (char)strtol(hex, NULL, 16);
                code += c;
                i += 2;
            } else {
                code += encoded[i];
            }
        }
    }
    
    if (code.empty()) {
        return "HTTP/1.1 400 Bad Request\r\nContent-Type: application/json\r\n\r\n{\"success\":false,\"error\":\"No code provided\"}";
    }
    
    std::string jsonResult = analyzeCode(code);
    
    std::stringstream response;
    response << "HTTP/1.1 200 OK\r\n";
    response << "Content-Type: application/json\r\n";
    response << "Content-Length: " << jsonResult.length() << "\r\n";
    response << "Access-Control-Allow-Origin: *\r\n";
    response << "Connection: close\r\n";
    response << "\r\n";
    response << jsonResult;
    
    return response.str();
}

int main() {
    std::cout << "========================================\n";
    std::cout << "   TASKSCRIPT SERVER - PROYECTO 2       \n";
    std::cout << "========================================\n";
    std::cout << "Servidor web iniciado en puerto 8080\n";
    std::cout << "Abra su navegador en: http://localhost:8080\n";
    std::cout << "Presione Ctrl+C para detener el servidor\n";
    std::cout << "========================================\n";

#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Error al inicializar Winsock" << std::endl;
        return 1;
    }
#endif

    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        std::cerr << "Error al crear socket" << std::endl;
        return 1;
    }
    
    int opt = 1;
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt));
    
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(8080);
    
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Error al bindear puerto 8080" << std::endl;
        return 1;
    }
    
    if (listen(serverSocket, 10) < 0) {
        std::cerr << "Error al escuchar" << std::endl;
        return 1;
    }
    
    while (true) {
        struct sockaddr_in clientAddr;
        socklen_t clientLen = sizeof(clientAddr);
        int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientLen);
        
        if (clientSocket < 0) continue;
        
        char buffer[65536] = {0};
        int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        
        if (bytesRead > 0) {
            std::string request(buffer);
            std::string response;
            
            // Parsear método y ruta
            if (request.find("POST /analyze") == 0) {
                size_t bodyStart = request.find("\r\n\r\n");
                if (bodyStart != std::string::npos) {
                    std::string body = request.substr(bodyStart + 4);
                    response = handleAnalyze(body);
                } else {
                    response = "HTTP/1.1 400 Bad Request\r\n\r\n";
                }
            } else {
                // GET - servir archivo estático
                std::string path = "/";
                size_t pathStart = request.find("GET /") + 5;
                size_t pathEnd = request.find(" ", pathStart);
                if (pathStart != std::string::npos && pathEnd != std::string::npos) {
                    path = request.substr(pathStart, pathEnd - pathStart);
                }
                response = serveFile(path);
            }
            
            send(clientSocket, response.c_str(), response.length(), 0);
        }
        
#ifdef _WIN32
        closesocket(clientSocket);
#else
        close(clientSocket);
#endif
    }
    
#ifdef _WIN32
    closesocket(serverSocket);
    WSACleanup();
#else
    close(serverSocket);
#endif
    
    return 0;
}