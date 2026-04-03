echo ========================================
echo Limpiando archivos temporales
echo ========================================
echo.

del *.o 2>nul
del *.exe 2>nul
del *.dll 2>nul
del compile_*.bat 2>nul
del build_*.bat 2>nul
del run.bat 2>nul
del test_*.cpp 2>nul
del test_*.exe 2>nul
del Programa.* 2>nul
del src\main_gui_debug.cpp 2>nul
del src\main.cpp 2>nul
rmdir src\gui\reports 2>nul

echo.
echo ========================================
echo Limpieza completada
echo ========================================
echo.
echo Archivos conservados:
echo   - include/ (todos los .h)
echo   - src/ (todos los .cpp)
echo   - build/MedLexer.exe
echo   - reports/*.html
echo   - CMakeLists.txt
echo   - hospital.dot
echo.
pause