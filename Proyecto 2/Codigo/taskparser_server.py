#!/usr/bin/env python3
from http.server import HTTPServer, BaseHTTPRequestHandler
import urllib.parse
import json
import subprocess
import os
import time

class TaskScriptHandler(BaseHTTPRequestHandler):
    
    def do_HEAD(self):
        """Manejar peticiones HEAD para verificar conectividad"""
        if self.path == '/':
            self.send_response(200)
            self.send_header('Content-Type', 'text/html')
            self.end_headers()
        else:
            self.send_response(404)
            self.end_headers()
    
    def do_GET(self):
        print(f"GET: {self.path}")
        
        if self.path == '/' or self.path == '/index.html':
            self.serve_file('web/index.html', 'text/html')
        elif self.path == '/style.css':
            self.serve_file('web/style.css', 'text/css')
        elif self.path == '/script.js':
            self.serve_file('web/script.js', 'application/javascript')
        elif 'reporte' in self.path:
            self.serve_file('web/' + self.path.lstrip('/'), 'text/html')
        else:
            self.send_response(404)
            self.end_headers()
    
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
    
    def do_POST(self):
        if self.path == '/analyze':
            print("\n=== ANALIZANDO ===")
            
            content_length = int(self.headers['Content-Length'])
            post_data = self.rfile.read(content_length)
            decoded = post_data.decode('utf-8')
            params = urllib.parse.parse_qs(decoded)
            code = params.get('code', [''])[0]
            
            print(f"¿Contiene '$'? {'$' in code}")
            print(f"Longitud: {len(code)}")
            
            # Guardar archivo
            temp_file = 'temp_analysis.task'
            with open(temp_file, 'w', encoding='utf-8') as f:
                f.write(code)
            
            # Verificar
            with open(temp_file, 'r', encoding='utf-8') as f:
                contenido = f.read()
                print(f"Archivo guardado: {len(contenido)} bytes")
                print(f"¿Contiene '$' en archivo? {'$' in contenido}")
            
            # Limpiar JSON anterior
            if os.path.exists('resultados.json'):
                os.remove('resultados.json')
            
            # Ejecutar analizador
            result = subprocess.run(
                ['.\\build\\taskparser.exe', temp_file, 'reporte'],  # Sin 'web/'
                capture_output=True,
                text=True,
                timeout=30
)
            
            print(f"Código de salida: {result.returncode}")
            
            # Leer resultados
            tokens = []
            errors = []
            dot_code = ""
            
            if os.path.exists('resultados.json'):
                with open('resultados.json', 'r', encoding='utf-8') as f:
                    data = json.load(f)
                    tokens = data.get('tokens', [])
                    errors = data.get('errors', [])  # <-- TODOS
                    dot_code = data.get('dotCode', '')

                print("JSON leído correctamente")
                print(json.dumps(data, indent=2, ensure_ascii=False))
                print(f"Errores enviados: {len(errors)}")
            else:
                print("ERROR: No se encontró resultados.json")
            
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
            
            print(f"Enviados {len(errors)} errores\n")

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