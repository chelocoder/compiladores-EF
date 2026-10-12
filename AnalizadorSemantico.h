#ifndef ANALIZADOR_SEMANTICO_H
#define ANALIZADOR_SEMANTICO_H

#include "AST.h"
#include "Token.h"
#include <unordered_map>
#include <vector>

struct Simbolo {
    std::string nombre;
    TipoDato tipo;
    int lineaDeclaracion;
};

class AnalizadorSemantico {
private:
    std::unordered_map<std::string, Simbolo> tablaSimbolos;
    std::vector<ErrorCompilador> errores;
    std::vector<bool> sentenciasValidas;

    TipoDato inferirTipo(const std::shared_ptr<NodoExpr>& expr, bool& valido);
    bool esNumerico(TipoDato tipo) const;
    bool compatible(TipoDato destino, TipoDato origen) const;
    void registrarError(int linea, int columna, const std::string& lexema, const std::string& mensaje);

public:
    void analizar(const std::vector<Sentencia>& sentencias);
    const std::unordered_map<std::string, Simbolo>& obtenerTablaSimbolos() const;
    const std::vector<ErrorCompilador>& obtenerErrores() const;
    const std::vector<bool>& obtenerSentenciasValidas() const;
};

#endif
