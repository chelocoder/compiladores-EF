#ifndef LEXER_H
#define LEXER_H

#include "Token.h"
#include <string>
#include <vector>

class Lexer {
private:
    std::string codigo;
    size_t posicion;
    int linea;
    int columna;
    std::vector<ErrorCompilador> errores;

    bool fin() const;
    char actual() const;
    char siguiente() const;
    void avanzar();
    void registrarError(const std::string& mensaje, int lin, int col, const std::string& lexema);

public:
    explicit Lexer(const std::string& codigoFuente);
    std::vector<Token> analizar();
    const std::vector<ErrorCompilador>& obtenerErrores() const;
};

#endif
