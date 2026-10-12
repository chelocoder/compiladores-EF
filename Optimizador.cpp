#include "Optimizador.h"
#include <cmath>
#include <cstdlib>
#include <iomanip>
#include <sstream>

bool Optimizador::esTemporal(const std::string& texto) {
    if (texto.size() < 2 || texto[0] != 't') return false;
    for (size_t i = 1; i < texto.size(); ++i) {
        if (texto[i] < '0' || texto[i] > '9') return false;
    }
    return true;
}

bool Optimizador::esNumero(const std::string& texto) {
    if (texto.empty()) return false;
    char* fin = nullptr;
    std::strtod(texto.c_str(), &fin);
    return fin && *fin == '\0';
}

double Optimizador::aNumero(const std::string& texto) {
    return std::strtod(texto.c_str(), nullptr);
}

std::string Optimizador::numeroAString(double valor) {
    if (std::fabs(valor - std::round(valor)) < 1e-9) {
        return std::to_string(static_cast<long long>(std::llround(valor)));
    }
    std::ostringstream out;
    out << std::fixed << std::setprecision(6) << valor;
    std::string s = out.str();
    while (!s.empty() && s.back() == '0') s.pop_back();
    if (!s.empty() && s.back() == '.') s.pop_back();
    return s;
}

std::string Optimizador::resolver(
    const std::string& valor,
    const std::unordered_map<std::string, std::string>& reemplazos) {

    std::string actual = valor;
    for (int i = 0; i < 20; ++i) {
        auto it = reemplazos.find(actual);
        if (it == reemplazos.end() || it->second == actual) break;
        actual = it->second;
    }
    return actual;
}

std::vector<InstruccionTAC> Optimizador::optimizar(const std::vector<InstruccionTAC>& entrada) const {
    std::vector<InstruccionTAC> salida;
    std::unordered_map<std::string, std::string> reemplazos;

    for (const auto& original : entrada) {
        InstruccionTAC i = original;
        i.arg1 = resolver(i.arg1, reemplazos);
        if (i.esOperacion) i.arg2 = resolver(i.arg2, reemplazos);

        if (i.esOperacion) {
            std::string simplificado;
            bool simplifica = false;

            if (esNumero(i.arg1) && esNumero(i.arg2)) {
                double a = aNumero(i.arg1);
                double b = aNumero(i.arg2);
                double r = 0.0;
                if (i.operador == "+") r = a + b;
                else if (i.operador == "-") r = a - b;
                else if (i.operador == "*") r = a * b;
                else if (i.operador == "/" && std::fabs(b) > 1e-12) r = a / b;
                else {
                    salida.push_back(i);
                    continue;
                }
                simplificado = numeroAString(r);
                simplifica = true;
            } else if (i.operador == "+" && i.arg2 == "0") {
                simplificado = i.arg1; simplifica = true;
            } else if (i.operador == "+" && i.arg1 == "0") {
                simplificado = i.arg2; simplifica = true;
            } else if (i.operador == "-" && i.arg2 == "0") {
                simplificado = i.arg1; simplifica = true;
            } else if (i.operador == "*" && i.arg2 == "1") {
                simplificado = i.arg1; simplifica = true;
            } else if (i.operador == "*" && i.arg1 == "1") {
                simplificado = i.arg2; simplifica = true;
            } else if (i.operador == "*" && (i.arg1 == "0" || i.arg2 == "0")) {
                simplificado = "0"; simplifica = true;
            } else if (i.operador == "/" && i.arg2 == "1") {
                simplificado = i.arg1; simplifica = true;
            }

            if (simplifica && esTemporal(i.destino)) {
                reemplazos[i.destino] = simplificado;
                continue;
            }

            if (simplifica) {
                salida.push_back({i.destino, simplificado, "", "", false});
                continue;
            }

            salida.push_back(i);
            continue;
        }

        std::string valor = resolver(i.arg1, reemplazos);
        if (esTemporal(i.destino)) {
            reemplazos[i.destino] = valor;
        } else {
            salida.push_back({i.destino, valor, "", "", false});
        }
    }

    return salida;
}
