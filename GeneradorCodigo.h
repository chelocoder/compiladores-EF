#ifndef GENERADOR_CODIGO_H
#define GENERADOR_CODIGO_H

#include "AST.h"
#include <string>
#include <vector>

struct InstruccionTAC {
    std::string destino;
    std::string arg1;
    std::string operador;
    std::string arg2;
    bool esOperacion = false;
};

class GeneradorCodigo {
private:
    int contadorTemporales;
    std::vector<InstruccionTAC> instrucciones;

    std::string nuevoTemporal();
    std::string generarExpresion(const std::shared_ptr<NodoExpr>& expr);

public:
    std::vector<InstruccionTAC> generar(const std::vector<Sentencia>& sentencias,
                                        const std::vector<bool>& validas);
    static std::string formatear(const InstruccionTAC& instruccion);
};

#endif
