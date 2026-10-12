#ifndef AST_H
#define AST_H

#include <memory>
#include <string>

enum class TipoDato {
    Entero,
    Real,
    Cadena,
    Desconocido
};

inline std::string nombreTipoDato(TipoDato tipo) {
    switch (tipo) {
        case TipoDato::Entero: return "int";
        case TipoDato::Real: return "float";
        case TipoDato::Cadena: return "string";
        default: return "desconocido";
    }
}

enum class ClaseExpr {
    LiteralEntero,
    LiteralReal,
    LiteralCadena,
    Identificador,
    Binaria
};

struct NodoExpr {
    ClaseExpr clase;
    std::string valor;
    char operador = '\0';
    std::shared_ptr<NodoExpr> izquierdo;
    std::shared_ptr<NodoExpr> derecho;
    int linea = 0;
    int columna = 0;
};

enum class TipoSentencia {
    Declaracion,
    Asignacion
};

struct Sentencia {
    TipoSentencia tipo;
    TipoDato tipoDeclarado = TipoDato::Desconocido;
    std::string identificador;
    std::shared_ptr<NodoExpr> expresion;
    int linea = 0;
    int columna = 0;
};

#endif
