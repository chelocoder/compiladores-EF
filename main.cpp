#include "AnalizadorSemantico.h"
#include "GeneradorCodigo.h"
#include "Lexer.h"
#include "Optimizador.h"
#include "Parser.h"
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

struct CasoPrueba {
    std::string nombre;
    std::string codigo;
    bool debeSerValido;
    bool mostrarTokens = false;
    bool mostrarTabla = false;
    bool mostrarIntermedio = false;
    bool mostrarOptimizacion = false;
};

static void imprimirErrores(const std::vector<ErrorCompilador>& errores) {
    if (errores.empty()) return;
    for (const auto& e : errores) {
        std::cout << "  [" << e.fase << "] linea " << e.linea
                  << ", columna " << e.columna
                  << ", lexema '" << e.lexema << "': "
                  << e.mensaje << "\n";
    }
}

static void imprimirTokens(const std::vector<Token>& tokens) {
    std::cout << "Tokens:\n";
    for (const auto& t : tokens) {
        if (t.tipo == TipoToken::FIN_ARCHIVO) continue;
        std::cout << "  <" << nombreTipoToken(t.tipo) << ", '" << t.lexema
                  << "'> [" << t.linea << ":" << t.columna << "]\n";
    }
}

static void imprimirTabla(const std::unordered_map<std::string, Simbolo>& tabla) {
    std::vector<Simbolo> simbolos;
    for (const auto& par : tabla) simbolos.push_back(par.second);
    std::sort(simbolos.begin(), simbolos.end(), [](const Simbolo& a, const Simbolo& b) {
        return a.lineaDeclaracion < b.lineaDeclaracion;
    });

    std::cout << "Tabla de simbolos:\n";
    std::cout << "  " << std::left << std::setw(18) << "Identificador"
              << std::setw(10) << "Tipo" << "Linea\n";
    for (const auto& s : simbolos) {
        std::cout << "  " << std::left << std::setw(18) << s.nombre
                  << std::setw(10) << nombreTipoDato(s.tipo)
                  << s.lineaDeclaracion << "\n";
    }
}

static bool procesar(const CasoPrueba& caso) {
    std::cout << "\n============================================================\n";
    std::cout << caso.nombre << "\n";
    std::cout << "============================================================\n";
    std::cout << "Codigo fuente:\n" << caso.codigo << "\n";

    Lexer lexer(caso.codigo);
    std::vector<Token> tokens = lexer.analizar();

    Parser parser(tokens);
    parser.analizar();

    AnalizadorSemantico semantico;
    semantico.analizar(parser.obtenerSentencias());

    if (caso.mostrarTokens) imprimirTokens(tokens);

    if (!lexer.obtenerErrores().empty()) {
        std::cout << "Errores lexicos:\n";
        imprimirErrores(lexer.obtenerErrores());
    }
    if (!parser.obtenerErrores().empty()) {
        std::cout << "Errores sintacticos:\n";
        imprimirErrores(parser.obtenerErrores());
    }
    if (!semantico.obtenerErrores().empty()) {
        std::cout << "Errores semanticos:\n";
        imprimirErrores(semantico.obtenerErrores());
    }

    if (caso.mostrarTabla) imprimirTabla(semantico.obtenerTablaSimbolos());

    bool sinErrores = lexer.obtenerErrores().empty() &&
                      parser.obtenerErrores().empty() &&
                      semantico.obtenerErrores().empty();

    if (sinErrores && (caso.mostrarIntermedio || caso.mostrarOptimizacion)) {
        GeneradorCodigo generador;
        auto tac = generador.generar(parser.obtenerSentencias(),
                                     semantico.obtenerSentenciasValidas());

        std::cout << "Codigo de tres direcciones - ANTES:\n";
        for (const auto& i : tac) {
            std::cout << "  " << GeneradorCodigo::formatear(i) << "\n";
        }

        if (caso.mostrarOptimizacion) {
            Optimizador optimizador;
            auto optimizado = optimizador.optimizar(tac);
            std::cout << "Codigo optimizado - DESPUES:\n";
            for (const auto& i : optimizado) {
                std::cout << "  " << GeneradorCodigo::formatear(i) << "\n";
            }
        }
    }

    bool obtenidoValido = sinErrores;
    bool ok = obtenidoValido == caso.debeSerValido;
    std::cout << "Esperado: " << (caso.debeSerValido ? "VALIDO" : "INVALIDO") << "\n";
    std::cout << "Obtenido: " << (obtenidoValido ? "VALIDO" : "INVALIDO") << "\n";
    std::cout << "Prueba: " << (ok ? "OK" : "FALLO") << "\n";
    return ok;
}

static std::string leerArchivo(const std::string& ruta) {
    std::ifstream archivo(ruta);
    if (!archivo) return "";
    std::ostringstream buffer;
    buffer << archivo.rdbuf();
    return buffer.str();
}

int main(int argc, char* argv[]) {
    if (argc > 1) {
        std::string codigo = leerArchivo(argv[1]);
        if (codigo.empty()) {
            std::cerr << "No se pudo leer el archivo: " << argv[1] << "\n";
            return 1;
        }
        CasoPrueba archivo{"Analisis de archivo", codigo, true, true, true, true, true};
        procesar(archivo);
        return 0;
    }

    std::vector<CasoPrueba> casos = {
        {
            "CASO 1 - Declaraciones validas int y float",
            "int cantidad = 4;\nfloat tarifa = 12.5;",
            true, false, true, false, false
        },
        {
            "CASO 2 - Literal string entre comillas dobles",
            "string ciudad = \"Huancayo\";",
            true, true, true, false, false
        },
        {
            "CASO 3 - Declaracion duplicada",
            "int cantidad = 4;\nint cantidad = 8;",
            false, false, true, false, false
        },
        {
            "CASO 4 - Identificador no declarado",
            "int total = precio;",
            false, false, false, false, false
        },
        {
            "CASO 5 - Error sintactico",
            "int cantidad 10;",
            false, false, false, false, false
        },
        {
            "CASO 6 - Error lexico y continuacion del analisis",
            "int lote = 10 @;\nfloat tarifa = 8.5;",
            false, false, true, false, false
        },
        {
            "CASO 7 - Codigo de varias lineas",
            "int cantidad = 3;\nfloat precio = 12.5;\nfloat subtotal = precio * cantidad;\nstring producto = \"MIEL_MANTARO\";",
            true, false, true, false, false
        },
        {
            "CASO 8 - Generacion de codigo de tres direcciones",
            "float distancia = 120.0;\nfloat tarifa = 1.5;\nfloat peaje = 8.0;\nfloat costo = distancia * tarifa + peaje;",
            true, false, true, true, false
        },
        {
            "CASO 9 - Optimizacion local por plegado de constantes",
            "int resultado = 2 + 3;",
            true, false, true, true, true
        },
        {
            "CASO 10 - Compatibilidad de tipos",
            "int edad = 20.5;",
            false, false, false, false, false
        },
        {
            "CASO 11 - Optimizacion de identidades algebraicas",
            "int resultado = 5 * 1 + 0;",
            true, false, false, true, true
        }
    };

    int correctas = 0;
    for (const auto& caso : casos) {
        if (procesar(caso)) correctas++;
    }

    std::cout << "\n============================================================\n";
    std::cout << "RESUMEN DE PRUEBAS\n";
    std::cout << "Correctas: " << correctas << " de " << casos.size() << "\n";
    std::cout << "============================================================\n";

    return correctas == static_cast<int>(casos.size()) ? 0 : 2;
}
