#include "Lexer.h"
#include <cctype>

Lexer::Lexer(const std::string& codigoFuente)
    : codigo(codigoFuente), posicion(0), linea(1), columna(1) {}

bool Lexer::fin() const {
    return posicion >= codigo.size();
}

char Lexer::actual() const {
    return fin() ? '\0' : codigo[posicion];
}

char Lexer::siguiente() const {
    return posicion + 1 >= codigo.size() ? '\0' : codigo[posicion + 1];
}

void Lexer::avanzar() {
    if (fin()) return;
    if (codigo[posicion] == '\n') {
        linea++;
        columna = 1;
    } else {
        columna++;
    }
    posicion++;
}

void Lexer::registrarError(const std::string& mensaje, int lin, int col, const std::string& lexema) {
    errores.push_back({"LEXICO", mensaje, lin, col, lexema});
}

std::vector<Token> Lexer::analizar() {
    errores.clear();
    std::vector<Token> tokens;

    while (!fin()) {
        char c = actual();

        if (std::isspace(static_cast<unsigned char>(c))) {
            avanzar();
            continue;
        }

        int inicioLinea = linea;
        int inicioColumna = columna;

        if (std::isalpha(static_cast<unsigned char>(c)) || c == '_') {
            std::string lexema;
            while (!fin() && (std::isalnum(static_cast<unsigned char>(actual())) || actual() == '_')) {
                lexema += actual();
                avanzar();
            }

            TipoToken tipo = TipoToken::IDENTIFICADOR;
            if (lexema == "int") tipo = TipoToken::KW_INT;
            else if (lexema == "float") tipo = TipoToken::KW_FLOAT;
            else if (lexema == "string") tipo = TipoToken::KW_STRING;

            tokens.push_back({tipo, lexema, inicioLinea, inicioColumna});
            continue;
        }

        if (std::isdigit(static_cast<unsigned char>(c))) {
            std::string lexema;
            bool esReal = false;

            while (!fin() && std::isdigit(static_cast<unsigned char>(actual()))) {
                lexema += actual();
                avanzar();
            }

            if (!fin() && actual() == '.' && std::isdigit(static_cast<unsigned char>(siguiente()))) {
                esReal = true;
                lexema += actual();
                avanzar();
                while (!fin() && std::isdigit(static_cast<unsigned char>(actual()))) {
                    lexema += actual();
                    avanzar();
                }
            }

            tokens.push_back({esReal ? TipoToken::REAL : TipoToken::ENTERO,
                              lexema, inicioLinea, inicioColumna});
            continue;
        }

        if (c == '"') {
            std::string lexema;
            lexema += c;
            avanzar();
            bool cerrada = false;

            while (!fin()) {
                if (actual() == '"') {
                    lexema += actual();
                    avanzar();
                    cerrada = true;
                    break;
                }
                if (actual() == '\n') {
                    break;
                }
                lexema += actual();
                avanzar();
            }

            if (!cerrada) {
                registrarError("Literal string sin comilla doble de cierre.",
                               inicioLinea, inicioColumna, lexema);
                tokens.push_back({TipoToken::DESCONOCIDO, lexema, inicioLinea, inicioColumna});
            } else {
                tokens.push_back({TipoToken::CADENA, lexema, inicioLinea, inicioColumna});
            }
            continue;
        }

        TipoToken tipo;
        bool reconocido = true;
        switch (c) {
            case '+': tipo = TipoToken::MAS; break;
            case '-': tipo = TipoToken::MENOS; break;
            case '*': tipo = TipoToken::MULTIPLICACION; break;
            case '/': tipo = TipoToken::DIVISION; break;
            case '=': tipo = TipoToken::ASIGNACION; break;
            case ';': tipo = TipoToken::PUNTO_COMA; break;
            case '(': tipo = TipoToken::PARENTESIS_IZQ; break;
            case ')': tipo = TipoToken::PARENTESIS_DER; break;
            default: reconocido = false; tipo = TipoToken::DESCONOCIDO; break;
        }

        std::string lexema(1, c);
        if (!reconocido) {
            registrarError("Caracter no reconocido.", inicioLinea, inicioColumna, lexema);
        }
        tokens.push_back({tipo, lexema, inicioLinea, inicioColumna});
        avanzar();
    }

    tokens.push_back({TipoToken::FIN_ARCHIVO, "<FIN>", linea, columna});
    return tokens;
}

const std::vector<ErrorCompilador>& Lexer::obtenerErrores() const {
    return errores;
}
