#include "GeneradorCodigo.h"

std::string GeneradorCodigo::nuevoTemporal() {
    return "t" + std::to_string(contadorTemporales++);
}

std::string GeneradorCodigo::generarExpresion(const std::shared_ptr<NodoExpr>& expr) {
    if (expr->clase == ClaseExpr::LiteralEntero ||
        expr->clase == ClaseExpr::LiteralReal ||
        expr->clase == ClaseExpr::LiteralCadena ||
        expr->clase == ClaseExpr::Identificador) {
        return expr->valor;
    }

    std::string izq = generarExpresion(expr->izquierdo);
    std::string der = generarExpresion(expr->derecho);
    std::string temporal = nuevoTemporal();
    instrucciones.push_back({temporal, izq, std::string(1, expr->operador), der, true});
    return temporal;
}

std::vector<InstruccionTAC> GeneradorCodigo::generar(const std::vector<Sentencia>& sentencias,
                                                     const std::vector<bool>& validas) {
    contadorTemporales = 1;
    instrucciones.clear();

    for (size_t i = 0; i < sentencias.size(); ++i) {
        if (i >= validas.size() || !validas[i]) continue;
        const Sentencia& s = sentencias[i];

        // La generacion solicitada se enfoca en expresiones aritmeticas.
        // Las asignaciones simples tambien se representan para conservar el flujo.
        std::string valor = generarExpresion(s.expresion);
        instrucciones.push_back({s.identificador, valor, "", "", false});
    }

    return instrucciones;
}

std::string GeneradorCodigo::formatear(const InstruccionTAC& i) {
    if (i.esOperacion) {
        return i.destino + " = " + i.arg1 + " " + i.operador + " " + i.arg2;
    }
    return i.destino + " = " + i.arg1;
}
