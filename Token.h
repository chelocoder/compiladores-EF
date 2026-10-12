#ifndef TOKEN_H
#define TOKEN_H

#include <string>

enum class TipoToken {
    KW_INT,
    KW_FLOAT,
    KW_STRING,
    IDENTIFICADOR,
    ENTERO,
    REAL,
    CADENA,
    MAS,
    MENOS,
    MULTIPLICACION,
    DIVISION,
    ASIGNACION,
    PUNTO_COMA,
    PARENTESIS_IZQ,
    PARENTESIS_DER,
    DESCONOCIDO,
    FIN_ARCHIVO
};

struct Token {
    TipoToken tipo;
    std::string lexema;
    int linea;
    int columna;
};

struct ErrorCompilador {
    std::string fase;
    std::string mensaje;
    int linea;
    int columna;
    std::string lexema;
};

inline std::string nombreTipoToken(TipoToken tipo) {
    switch (tipo) {
        case TipoToken::KW_INT: return "KW_INT";
        case TipoToken::KW_FLOAT: return "KW_FLOAT";
        case TipoToken::KW_STRING: return "KW_STRING";
        case TipoToken::IDENTIFICADOR: return "IDENTIFICADOR";
        case TipoToken::ENTERO: return "ENTERO";
        case TipoToken::REAL: return "REAL";
        case TipoToken::CADENA: return "CADENA";
        case TipoToken::MAS: return "MAS";
        case TipoToken::MENOS: return "MENOS";
        case TipoToken::MULTIPLICACION: return "MULTIPLICACION";
        case TipoToken::DIVISION: return "DIVISION";
        case TipoToken::ASIGNACION: return "ASIGNACION";
        case TipoToken::PUNTO_COMA: return "PUNTO_COMA";
        case TipoToken::PARENTESIS_IZQ: return "PARENTESIS_IZQ";
        case TipoToken::PARENTESIS_DER: return "PARENTESIS_DER";
        case TipoToken::DESCONOCIDO: return "DESCONOCIDO";
        case TipoToken::FIN_ARCHIVO: return "FIN_ARCHIVO";
    }
    return "DESCONOCIDO";
}

#endif
