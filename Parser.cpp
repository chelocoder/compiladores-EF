#include "Parser.h"

Parser::Parser(const std::vector<Token>& tokensEntrada)
    : tokens(tokensEntrada), posicion(0) {}

const Token& Parser::actual() const {
    return tokens[posicion];
}

bool Parser::es(TipoToken tipo) const {
    return actual().tipo == tipo;
}

void Parser::avanzar() {
    if (!es(TipoToken::FIN_ARCHIVO)) posicion++;
}

bool Parser::aceptar(TipoToken tipo) {
    if (es(tipo)) {
        avanzar();
        return true;
    }
    return false;
}

bool Parser::esperar(TipoToken tipo, const std::string& mensaje) {
    if (aceptar(tipo)) return true;
    registrarError(actual(), mensaje);
    return false;
}

void Parser::registrarError(const Token& token, const std::string& mensaje) {
    errores.push_back({"SINTACTICO", mensaje, token.linea, token.columna, token.lexema});
}

void Parser::sincronizar() {
    while (!es(TipoToken::FIN_ARCHIVO)) {
        if (es(TipoToken::PUNTO_COMA)) {
            avanzar();
            return;
        }
        if (es(TipoToken::KW_INT) || es(TipoToken::KW_FLOAT) ||
            es(TipoToken::KW_STRING) || es(TipoToken::IDENTIFICADOR)) {
            return;
        }
        avanzar();
    }
}

TipoDato Parser::tipoDesdeToken(TipoToken tipo) const {
    if (tipo == TipoToken::KW_INT) return TipoDato::Entero;
    if (tipo == TipoToken::KW_FLOAT) return TipoDato::Real;
    if (tipo == TipoToken::KW_STRING) return TipoDato::Cadena;
    return TipoDato::Desconocido;
}

bool Parser::analizar() {
    errores.clear();
    sentencias.clear();
    posicion = 0;

    while (!es(TipoToken::FIN_ARCHIVO)) {
        if (!analizarSentencia()) {
            sincronizar();
        }
    }

    return errores.empty();
}

bool Parser::analizarSentencia() {
    if (es(TipoToken::KW_INT) || es(TipoToken::KW_FLOAT) || es(TipoToken::KW_STRING)) {
        return analizarDeclaracion();
    }
    if (es(TipoToken::IDENTIFICADOR)) {
        return analizarAsignacion();
    }

    registrarError(actual(), "Se esperaba una declaracion o una asignacion.");
    return false;
}

bool Parser::analizarDeclaracion() {
    Token tokenTipo = actual();
    TipoDato tipo = tipoDesdeToken(tokenTipo.tipo);
    avanzar();

    if (!es(TipoToken::IDENTIFICADOR)) {
        registrarError(actual(), "Se esperaba un identificador despues del tipo.");
        return false;
    }
    Token id = actual();
    avanzar();

    if (!esperar(TipoToken::ASIGNACION, "Se esperaba '=' despues del identificador.")) {
        return false;
    }

    std::shared_ptr<NodoExpr> expr = analizarExpresion();
    if (!expr) return false;

    if (!esperar(TipoToken::PUNTO_COMA, "Se esperaba ';' al final de la declaracion.")) {
        return false;
    }

    sentencias.push_back({TipoSentencia::Declaracion, tipo, id.lexema, expr, id.linea, id.columna});
    return true;
}

bool Parser::analizarAsignacion() {
    Token id = actual();
    avanzar();

    if (!esperar(TipoToken::ASIGNACION, "Se esperaba '=' despues del identificador.")) {
        return false;
    }

    std::shared_ptr<NodoExpr> expr = analizarExpresion();
    if (!expr) return false;

    if (!esperar(TipoToken::PUNTO_COMA, "Se esperaba ';' al final de la asignacion.")) {
        return false;
    }

    sentencias.push_back({TipoSentencia::Asignacion, TipoDato::Desconocido,
                          id.lexema, expr, id.linea, id.columna});
    return true;
}

std::shared_ptr<NodoExpr> Parser::analizarExpresion() {
    auto izquierdo = analizarTermino();
    if (!izquierdo) return nullptr;

    while (es(TipoToken::MAS) || es(TipoToken::MENOS)) {
        Token op = actual();
        avanzar();
        auto derecho = analizarTermino();
        if (!derecho) return nullptr;

        auto nodo = std::make_shared<NodoExpr>();
        nodo->clase = ClaseExpr::Binaria;
        nodo->operador = op.lexema[0];
        nodo->izquierdo = izquierdo;
        nodo->derecho = derecho;
        nodo->linea = op.linea;
        nodo->columna = op.columna;
        izquierdo = nodo;
    }
    return izquierdo;
}

std::shared_ptr<NodoExpr> Parser::analizarTermino() {
    auto izquierdo = analizarFactor();
    if (!izquierdo) return nullptr;

    while (es(TipoToken::MULTIPLICACION) || es(TipoToken::DIVISION)) {
        Token op = actual();
        avanzar();
        auto derecho = analizarFactor();
        if (!derecho) return nullptr;

        auto nodo = std::make_shared<NodoExpr>();
        nodo->clase = ClaseExpr::Binaria;
        nodo->operador = op.lexema[0];
        nodo->izquierdo = izquierdo;
        nodo->derecho = derecho;
        nodo->linea = op.linea;
        nodo->columna = op.columna;
        izquierdo = nodo;
    }
    return izquierdo;
}

std::shared_ptr<NodoExpr> Parser::analizarFactor() {
    Token token = actual();
    auto nodo = std::make_shared<NodoExpr>();
    nodo->linea = token.linea;
    nodo->columna = token.columna;

    if (aceptar(TipoToken::ENTERO)) {
        nodo->clase = ClaseExpr::LiteralEntero;
        nodo->valor = token.lexema;
        return nodo;
    }
    if (aceptar(TipoToken::REAL)) {
        nodo->clase = ClaseExpr::LiteralReal;
        nodo->valor = token.lexema;
        return nodo;
    }
    if (aceptar(TipoToken::CADENA)) {
        nodo->clase = ClaseExpr::LiteralCadena;
        nodo->valor = token.lexema;
        return nodo;
    }
    if (aceptar(TipoToken::IDENTIFICADOR)) {
        nodo->clase = ClaseExpr::Identificador;
        nodo->valor = token.lexema;
        return nodo;
    }
    if (aceptar(TipoToken::PARENTESIS_IZQ)) {
        auto expr = analizarExpresion();
        if (!expr) return nullptr;
        if (!esperar(TipoToken::PARENTESIS_DER, "Se esperaba ')' para cerrar la expresion.")) {
            return nullptr;
        }
        return expr;
    }

    registrarError(actual(), "Se esperaba un valor, identificador o expresion.");
    return nullptr;
}

const std::vector<ErrorCompilador>& Parser::obtenerErrores() const {
    return errores;
}

const std::vector<Sentencia>& Parser::obtenerSentencias() const {
    return sentencias;
}
