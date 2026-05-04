#!/usr/bin/env python3
# taskparser_server.py - Servidor web para TaskScript

from http.server import HTTPServer, BaseHTTPRequestHandler
import urllib.parse
import json
import subprocess
import tempfile
import os
import re

class TaskScriptHandler(BaseHTTPRequestHandler):
    
    def do_GET(self):
        print(f"GET: {self.path}")
        
        if self.path == '/' or self.path == '/index.html':
            self.serve_file('web/index.html', 'text/html')
        elif self.path == '/style.css':
            self.serve_file('web/style.css', 'text/css')
        elif self.path == '/script.js':
            self.serve_file('web/script.js', 'application/javascript')
        elif self.path == '/reporte_kanban.html':
            self.serve_file('reporte_kanban.html', 'text/html')
        elif self.path == '/reporte_responsable.html':
            self.serve_file('reporte_responsable.html', 'text/html')
        elif self.path == '/reporte_tokens.html':
            self.serve_file('reporte_tokens.html', 'text/html')
        else:
            self.send_response(404)
            self.end_headers()
            self.wfile.write(b'404 - Archivo no encontrado')
    
    def serve_file(self, filepath, content_type):
        try:
            with open(filepath, 'rb') as f:
                content = f.read()
            self.send_response(200)
            self.send_header('Content-Type', content_type)
            self.send_header('Content-Length', len(content))
            self.end_headers()
            self.wfile.write(content)
        except FileNotFoundError:
            self.send_response(404)
            self.end_headers()
            self.wfile.write(b'Archivo no encontrado')
    
    def do_POST(self):
        if self.path == '/analyze':
            print("POST /analyze - Recibiendo código")
            
            content_length = int(self.headers['Content-Length'])
            post_data = self.rfile.read(content_length)
            params = urllib.parse.parse_qs(post_data.decode('utf-8'))
            code = params.get('code', [''])[0]
            
            print(f"Longitud del código: {len(code)} caracteres")
            
            # Guardar código temporal
            with tempfile.NamedTemporaryFile(mode='w', suffix='.task', delete=False, encoding='utf-8') as f:
                f.write(code)
                temp_file = f.name
            
            print(f"Archivo temporal: {temp_file}")
            
            # Ejecutar analizador C++
            try:
                result = subprocess.run(
                    ['.\\build\\taskparser.exe', temp_file, 'web/reporte'],
                    capture_output=True,
                    text=True,
                    timeout=30
                )
                print(f"Analizador ejecutado. Código de salida: {result.returncode}")
                print(f"STDOUT: {result.stdout[:200]}...")
                if result.stderr:
                    print(f"STDERR: {result.stderr[:200]}...")
            except Exception as e:
                print(f"Error al ejecutar analizador: {e}")
                self.send_error_response(f"Error al ejecutar analizador: {str(e)}")
                os.unlink(temp_file)
                return
            
            # Leer resultados del JSON
            tokens = []
            errors = []
            dot_code = ""
            
            json_file = 'resultados.json'
            if os.path.exists(json_file):
                try:
                    with open(json_file, 'r', encoding='utf-8') as f:
                        data = json.load(f)
                        tokens = data.get('tokens', [])
                        errors = data.get('errors', [])
                        dot_code = data.get('dotCode', '')
                    print(f"JSON cargado: {len(tokens)} tokens, {len(errors)} errores")
                except Exception as e:
                    print(f"Error leyendo JSON: {e}")
            
            # Limpiar
            os.unlink(temp_file)
            
            # Enviar respuesta
            response = json.dumps({
                'success': True,
                'tokens': tokens,
                'errors': errors,
                'dotCode': dot_code
            }, ensure_ascii=False)
            
            self.send_response(200)
            self.send_header('Content-Type', 'application/json; charset=utf-8')
            self.send_header('Access-Control-Allow-Origin', '*')
            self.send_header('Content-Length', len(response.encode('utf-8')))
            self.end_headers()
            self.wfile.write(response.encode('utf-8'))
            print("Respuesta enviada")
    
    def send_error_response(self, message):
        response = json.dumps({'success': False, 'error': message})
        self.send_response(500)
        self.send_header('Content-Type', 'application/json')
        self.end_headers()
        self.wfile.write(response.encode('utf-8'))
    
    def log_message(self, format, *args):
        print(f"{self.address_string()} - {format % args}")

def main():
    port = 8080
    server = HTTPServer(('localhost', port), TaskScriptHandler)
    print("=" * 50)
    print("   TASKSCRIPT SERVER - PROYECTO 2")
    print("=" * 50)
    print(f"Servidor web iniciado en http://localhost:{port}")
    print("Presione Ctrl+C para detener el servidor")
    print("=" * 50)
    
    try:
        server.serve_forever()
    except KeyboardInterrupt:
        print("\nServidor detenido")
        server.shutdown()

if __name__ == '__main__':
    main()