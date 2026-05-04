# test_server.py - Servidor simple para pruebas
from http.server import HTTPServer, BaseHTTPRequestHandler
import urllib.parse
import json
import subprocess
import tempfile
import os

class TaskScriptHandler(BaseHTTPRequestHandler):
    def do_GET(self):
        if self.path == '/':
            self.send_response(200)
            self.send_header('Content-type', 'text/html')
            self.end_headers()
            with open('web/index.html', 'rb') as f:
                self.wfile.write(f.read())
        elif self.path.endswith('.css'):
            self.send_response(200)
            self.send_header('Content-type', 'text/css')
            self.end_headers()
            with open('web/style.css', 'rb') as f:
                self.wfile.write(f.read())
        elif self.path.endswith('.js'):
            self.send_response(200)
            self.send_header('Content-type', 'application/javascript')
            self.end_headers()
            with open('web/script.js', 'rb') as f:
                self.wfile.write(f.read())
        else:
            self.send_response(404)
            self.end_headers()
    
    def do_POST(self):
        if self.path == '/analyze':
            content_length = int(self.headers['Content-Length'])
            post_data = self.rfile.read(content_length)
            params = urllib.parse.parse_qs(post_data.decode('utf-8'))
            code = params.get('code', [''])[0]
            
            # Guardar código temporal
            with tempfile.NamedTemporaryFile(mode='w', suffix='.task', delete=False) as f:
                f.write(code)
                temp_file = f.name
            
            # Ejecutar analizador
            result = subprocess.run(['build/taskparser.exe', temp_file], capture_output=True, text=True)
            
            # Leer resultados
            tokens = []
            errors = []
            dot_code = ""
            
            # Leer JSON si existe
            if os.path.exists('resultados.json'):
                with open('resultados.json', 'r') as f:
                    data = json.load(f)
                    tokens = data.get('tokens', [])
                    errors = data.get('errors', [])
                    dot_code = data.get('dotCode', '')
            
            os.unlink(temp_file)
            
            response = json.dumps({
                'success': True,
                'tokens': tokens,
                'errors': errors,
                'dotCode': dot_code
            })
            
            self.send_response(200)
            self.send_header('Content-type', 'application/json')
            self.end_headers()
            self.wfile.write(response.encode('utf-8'))

if __name__ == '__main__':
    print("Servidor iniciado en http://localhost:8080")
    server = HTTPServer(('localhost', 8080), TaskScriptHandler)
    server.serve_forever()