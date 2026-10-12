# Evaluación Final - Compiladores

Mini compilador desarrollado en C++.

## Funcionalidades
- Análisis léxico.
- Análisis sintáctico.
- Soporte para int, float y string.
- Procesamiento de varias líneas.
- Acumulación de errores.
- Tabla de símbolos.
- Validaciones semánticas.
- Generación de código de tres direcciones.
- Optimización local mediante plegado de constantes.

## Compilación

cl /EHsc /std:c++17 main.cpp Lexer.cpp Parser.cpp AnalizadorSemantico.cpp GeneradorCodigo.cpp Optimizador.cpp

## Ejecución

.\main.exe
