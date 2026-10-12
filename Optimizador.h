#ifndef OPTIMIZADOR_H
#define OPTIMIZADOR_H

#include "GeneradorCodigo.h"
#include <string>
#include <unordered_map>
#include <vector>

class Optimizador {
private:
    static bool esTemporal(const std::string& texto);
    static bool esNumero(const std::string& texto);
    static double aNumero(const std::string& texto);
    static std::string numeroAString(double valor);
    static std::string resolver(const std::string& valor,
                                const std::unordered_map<std::string, std::string>& reemplazos);

public:
    std::vector<InstruccionTAC> optimizar(const std::vector<InstruccionTAC>& entrada) const;
};

#endif
