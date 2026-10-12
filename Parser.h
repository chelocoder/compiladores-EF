#ifndef PARSER_H
#define PARSER_H

#include "AST.h"
#include "Token.h"
#include <memory>
#include <vector>

class Parser {
private:
    std::vector<Token> tokens;
    size_t posicion;
    std::vector<ErrorCompilador> errores;
    std::vector<Sentencia> sentencias;

    const Token& actual() const;
    bool es(TipoToken tipo) const;
    bool aceptar(TipoToken tipo);
    bool esperar(TipoToken tipo, const std::string& mensaje);
    void avanzar();
    void registrarError(const Token& token, const std::string& mensaje);
    void sincronizar();

    bool analizarSentencia();
    bool analizarDeclaracion();
    bool analizarAsignacion();

    std::shared_ptr<NodoExpr> analizarExpresion();
    std::shared_ptr<NodoExpr> analizarTermino();
    std::shared_ptr<NodoExpr> analizarFactor();

    TipoDato tipoDesdeToken(TipoToken tipo) const;

public:
    explicit Parser(const std::vector<Token>& tokensEntrada);
    bool analizar();
    const std::vector<ErrorCompilador>& obtenerErrores() const;
    const std::vector<Sentencia>& obtenerSentencias() const;
};

#endif
