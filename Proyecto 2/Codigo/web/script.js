// Configuración
/*const API_URL = 'http://localhost:8080/analyze';*/

// Elementos DOM
const editor = document.getElementById('editor');
const fileInput = document.getElementById('fileInput');
const btnLoad = document.getElementById('btnLoad');
const btnAnalyze = document.getElementById('btnAnalyze');
const btnClear = document.getElementById('btnClear');
const btnSaveDot = document.getElementById('btnSaveDot');
const btnViewGraph = document.getElementById('btnViewGraph');
const btnKanban = document.getElementById('btnKanban');
const btnResponsable = document.getElementById('btnResponsable');
const btnTokens = document.getElementById('btnTokens');
const statusBar = document.getElementById('statusBar');
const tokenBody = document.getElementById('tokenBody');
const errorBody = document.getElementById('errorBody');
const dotCode = document.getElementById('dotCode');
const reportFrame = document.getElementById('reportFrame');

// Estado
let currentTokens = [];
let currentErrors = [];
let currentDotCode = '';

// Pestañas
const tabs = document.querySelectorAll('.tab-btn');
tabs.forEach(btn => {
    btn.addEventListener('click', () => {
        const tabId = btn.dataset.tab;
        document.querySelectorAll('.tab-btn').forEach(b => b.classList.remove('active'));
        document.querySelectorAll('.tab-content').forEach(c => c.classList.remove('active'));
        btn.classList.add('active');
        document.getElementById(`tab-${tabId}`).classList.add('active');
    });
});

// Cargar archivo - CORREGIDO
btnLoad.addEventListener('click', () => {
    fileInput.click();
});

fileInput.addEventListener('change', (e) => {
    const file = e.target.files[0];
    if (!file) return;
    
    const reader = new FileReader();
    reader.onload = (event) => {
        editor.value = event.target.result;
        statusBar.innerHTML = 'Archivo cargado: ' + file.name;
    };
    reader.readAsText(file);
    fileInput.value = '';
});

// Analizar código - Versión mejorada
btnAnalyze.addEventListener('click', async () => {
    const code = editor.value;
    if (!code.trim()) {
        statusBar.innerHTML = 'Ingrese código para analizar';
        return;
    }
    
    statusBar.innerHTML = 'Analizando código...';
    btnAnalyze.disabled = true;
    
    try {
        // Enviar como texto plano en lugar de URLSearchParams
        const response = await fetch('http://localhost:8080/analyze', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/x-www-form-urlencoded; charset=utf-8',
            },
            body: 'code=' + encodeURIComponent(code)
        });
        
        if (!response.ok) {
            throw new Error(`HTTP error! status: ${response.status}`);
        }
        
        const data = await response.json();
        
        if (data.success) {
            currentTokens = data.tokens || [];
            currentErrors = data.errors || [];
            currentDotCode = data.dotCode || '';
            
            updateTokenTable(currentTokens);
            updateErrorTable(currentErrors);
            dotCode.textContent = currentDotCode;
            
            statusBar.innerHTML = `Análisis completado: ${currentTokens.length} tokens, ${currentErrors.length} errores`;
            
            if (currentErrors.length > 0) {
                console.log("Errores detectados:", currentErrors);
            }
            
            loadReportInFrame('reporte_kanban.html');
        } else {
            statusBar.innerHTML = 'Error en el análisis';
        }
        
    } catch (error) {
        console.error('Error:', error);
        statusBar.innerHTML = ' Error de conexión. ¿El servidor está ejecutándose?';
        alert('No se pudo conectar al servidor.\n\nAsegúrese de ejecutar:\n.\\build\\taskparser.exe\n\nLuego recargue la página.');
    }
    
    btnAnalyze.disabled = false;
});

// Limpiar
btnClear.addEventListener('click', () => {
    editor.value = '';
    tokenBody.innerHTML = '';
    errorBody.innerHTML = '';
    dotCode.textContent = '';
    currentTokens = [];
    currentErrors = [];
    currentDotCode = '';
    statusBar.innerHTML = 'Datos limpiados';
});

// Guardar DOT
btnSaveDot.addEventListener('click', () => {
    if (!currentDotCode) {
        statusBar.innerHTML = 'No hay código DOT para guardar';
        return;
    }
    
    const blob = new Blob([currentDotCode], { type: 'text/plain' });
    const link = document.createElement('a');
    link.href = URL.createObjectURL(blob);
    link.download = 'arbol_derivacion.dot';
    link.click();
    URL.revokeObjectURL(link.href);
    statusBar.innerHTML = 'Archivo DOT guardado';
});

// Ver gráfico
btnViewGraph.addEventListener('click', () => {
    if (!currentDotCode) {
        statusBar.innerHTML = 'No hay código DOT para visualizar';
        return;
    }
    
    const encoded = encodeURIComponent(currentDotCode);
    const url = `https://dreampuf.github.io/GraphvizOnline/#${encoded}`;
    window.open(url, '_blank');
    statusBar.innerHTML = 'Abriendo visualizador de árbol';
});

// Abrir reportes - CORREGIDO
btnKanban.addEventListener('click', () => {
    window.open('reporte_kanban.html', '_blank');  // Sin 'web/'
});

btnResponsable.addEventListener('click', () => {
    window.open('reporte_responsable.html', '_blank');
});

btnTokens.addEventListener('click', () => {
    window.open('reporte_tokens.html', '_blank');
});


// Cargar reporte en el iframe
function loadReportInFrame(fileName) {
    if (reportFrame) {
        reportFrame.src = fileName;  // Sin 'web/'
    }
}

// Actualizar tabla de tokens
function updateTokenTable(tokens) {
    tokenBody.innerHTML = '';
    if (!tokens || tokens.length === 0) {
        const row = tokenBody.insertRow();
        row.insertCell(0).colSpan = 5;
        row.insertCell(0).textContent = 'No hay tokens para mostrar';
        return;
    }
    
    tokens.forEach((token, index) => {
        const row = tokenBody.insertRow();
        row.insertCell(0).textContent = token.no || (index + 1);
        row.insertCell(1).textContent = token.lexeme || '';
        row.insertCell(2).textContent = token.type || '';
        row.insertCell(3).textContent = token.line || 0;
        row.insertCell(4).textContent = token.column || 0;
    });
}

// Actualizar tabla de errores - Versión mejorada
function updateErrorTable(errors) {
    errorBody.innerHTML = '';
    if (!errors || errors.length === 0) {
        const row = errorBody.insertRow();
        row.insertCell(0).colSpan = 7;
        row.insertCell(0).textContent = 'No hay errores para mostrar';
        return;
    }
    
    console.log("Mostrando", errors.length, "errores:", errors);
    
    errors.forEach((error, idx) => {
        const row = errorBody.insertRow();
        if (error.severity === 'CRITICO') {
            row.classList.add('critico-row');
        } else if (error.severity === 'ERROR') {
            row.classList.add('error-row');
        }
        row.insertCell(0).textContent = error.no || (idx + 1);
        row.insertCell(1).textContent = error.lexeme || '';
        row.insertCell(2).textContent = error.type || '';
        row.insertCell(3).textContent = error.description || '';
        row.insertCell(4).textContent = error.line || 0;
        row.insertCell(5).textContent = error.column || 0;
        row.insertCell(6).textContent = error.severity || '';
    });
}
// Cargar reporte en el iframe
/*function loadReportInFrame(fileName) {
    if (reportFrame) {
        reportFrame.src = fileName;
    }
}
*/
// Verificar conexión con el servidor
async function checkServer() {
    try {
        const response = await fetch('http://localhost:8080/', { method: 'HEAD' });
        if (response.ok) {
            statusBar.innerHTML = ' Conectado al servidor. Listo para analizar.';
        }
    } catch (error) {
        statusBar.innerHTML = 'Servidor no disponible. Ejecute taskparser_server.exe';
    }
}

// Inicializar
checkServer();
setInterval(checkServer, 10000);