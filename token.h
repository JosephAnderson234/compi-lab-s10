#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include <ostream>

using namespace std;

class Token {
public:
    // Tipos de token
    enum Type {
        PLUS,    // +
        MINUS,   // -
        MUL,     // *
        DIV,     // /
        POW,     // **
        LPAREN,  // (
        RPAREN,  // )
        PRINT,
        WHILE,
        LET,
        BREAK,
        CONTINUE,
        DEF,
        EQUIV,
        DOSPUNTOS,
        ENDFUN,
        RETURN,
        DO,
        ENDWHILE,
        IF,
        FOR,
        TO,
        ENDFOR,
        VAR,
        COMA,
        ENDIF,
        THEN,
        ELSE,
        SEMICOL,
        ASSIGN,
        TRUE,
        FALSE,
        SQRT,    // sqrt
        NUM,     // Número
        ERR,     // Error
        ID,      // ID
        END      // Fin de entrada
    };

    // Atributos
    Type type;
    string text;

    // Constructores
    Token(Type type);
    Token(Type type, char c);
    Token(Type type, const string& source, int first, int last);

    // Sobrecarga de operadores de salida
    friend ostream& operator<<(ostream& outs, const Token& tok);
    friend ostream& operator<<(ostream& outs, const Token* tok);
};

#endif // TOKEN_H