#include "AnalizadorSemantico.h"

bool AnalizadorSemantico::esNumerico(TipoDato tipo) const {
    return tipo == TipoDato::Entero || tipo == TipoDato::Real;
}

bool AnalizadorSemantico::compatible(TipoDato destino, TipoDato origen) const {
    if (destino == origen) return true;
    // Regla elegida para este mini lenguaje: int puede promoverse a float.
    if (destino == TipoDato::Real && origen == TipoDato::Entero) return true;
    return false;
}

void AnalizadorSemantico::registrarError(int linea, int columna,
                                         const std::string& lexema,
                                         const std::string& mensaje) {
    errores.push_back({"SEMANTICO", mensaje, linea, columna, lexema});
}

TipoDato AnalizadorSemantico::inferirTipo(const std::shared_ptr<NodoExpr>& expr, bool& valido) {
    if (!expr) {
        valido = false;
        return TipoDato::Desconocido;
    }

    switch (expr->clase) {
        case ClaseExpr::LiteralEntero:
            return TipoDato::Entero;
        case ClaseExpr::LiteralReal:
            return TipoDato::Real;
        case ClaseExpr::LiteralCadena:
            return TipoDato::Cadena;
        case ClaseExpr::Identificador: {
            auto it = tablaSimbolos.find(expr->valor);
            if (it == tablaSimbolos.end()) {
                registrarError(expr->linea, expr->columna, expr->valor,
                               "Identificador usado sin declaracion previa.");
                valido = false;
                return TipoDato::Desconocido;
            }
            return it->second.tipo;
        }
        case ClaseExpr::Binaria: {
            bool validoIzq = true;
            bool validoDer = true;
            TipoDato izq = inferirTipo(expr->izquierdo, validoIzq);
            TipoDato der = inferirTipo(expr->derecho, validoDer);
            if (!validoIzq || !validoDer) {
                valido = false;
                return TipoDato::Desconocido;
            }

            if (!esNumerico(izq) || !esNumerico(der)) {
                registrarError(expr->linea, expr->columna, std::string(1, expr->operador),
                               "Operacion aritmetica incompatible con tipo string.");
                valido = false;
                return TipoDato::Desconocido;
            }

            if (izq == TipoDato::Real || der == TipoDato::Real) return TipoDato::Real;
            return TipoDato::Entero;
        }
    }

    valido = false;
    return TipoDato::Desconocido;
}

void AnalizadorSemantico::analizar(const std::vector<Sentencia>& sentencias) {
    tablaSimbolos.clear();
    errores.clear();
    sentenciasValidas.assign(sentencias.size(), true);

    for (size_t i = 0; i < sentencias.size(); ++i) {
        const Sentencia& s = sentencias[i];
        bool valida = true;

        if (s.tipo == TipoSentencia::Declaracion) {
            if (tablaSimbolos.find(s.identificador) != tablaSimbolos.end()) {
                registrarError(s.linea, s.columna, s.identificador,
                               "Declaracion duplicada en el mismo ambito.");
                sentenciasValidas[i] = false;
                continue;
            }

            TipoDato tipoExpr = inferirTipo(s.expresion, valida);
            if (valida && !compatible(s.tipoDeclarado, tipoExpr)) {
                registrarError(s.linea, s.columna, s.identificador,
                               "Tipo incompatible: no se puede asignar " +
                               nombreTipoDato(tipoExpr) + " a " +
                               nombreTipoDato(s.tipoDeclarado) + ".");
                valida = false;
            }

            if (valida) {
                tablaSimbolos[s.identificador] = {s.identificador, s.tipoDeclarado, s.linea};
            }
        } else {
            auto it = tablaSimbolos.find(s.identificador);
            if (it == tablaSimbolos.end()) {
                registrarError(s.linea, s.columna, s.identificador,
                               "Variable destino no declarada.");
                valida = false;
            }

            TipoDato tipoExpr = inferirTipo(s.expresion, valida);
            if (valida && it != tablaSimbolos.end() && !compatible(it->second.tipo, tipoExpr)) {
                registrarError(s.linea, s.columna, s.identificador,
                               "Tipo incompatible en asignacion.");
                valida = false;
            }
        }

        sentenciasValidas[i] = valida;
    }
}

const std::unordered_map<std::string, Simbolo>& AnalizadorSemantico::obtenerTablaSimbolos() const {
    return tablaSimbolos;
}

const std::vector<ErrorCompilador>& AnalizadorSemantico::obtenerErrores() const {
    return errores;
}

const std::vector<bool>& AnalizadorSemantico::obtenerSentenciasValidas() const {
    return sentenciasValidas;
}
