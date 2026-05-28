#include <iostream>
#include <stdexcept>
#include "token.h"
#include "scanner.h"
#include "ast.h"
#include "parser.h"

using namespace std;

// =============================
// Métodos de la clase Parser
// =============================

Parser::Parser(Scanner* sc) : scanner(sc) {
    previous = nullptr;
    current = scanner->nextToken();
    if (current->type == Token::ERR) {
        throw runtime_error("Error léxico");
    }
}

bool Parser::match(Token::Type ttype) {
    if (check(ttype)) {
        advance();
        return true;
    }
    return false;
}

bool Parser::check(Token::Type ttype) {
    if (isAtEnd()) return false;
    return current->type == ttype;
}

bool Parser::advance() {
    if (!isAtEnd()) {
        Token* temp = current;
        if (previous) delete previous;
        current = scanner->nextToken();
        previous = temp;

        if (check(Token::ERR)) {
            throw runtime_error("Error lexico");
        }
        return true;
    }
    return false;
}

bool Parser::isAtEnd() {
    return (current->type == Token::END);
}


// =============================
// Reglas gramaticales
// =============================

Programa* Parser::parseProgram() {
    Programa* ast = parseP();
    if (!isAtEnd()) {
        throw runtime_error("Error sintáctico");
    }
    cout << "Parseo exitoso" << endl;
    return ast;
}


Programa* Parser::parseP() {
    Programa* p =  new Programa();

    while(check(Token::VAR)){
        p->vdlist.push_back(parsevardec());
        match(Token::SEMICOL);
    }
    while(check(Token::DEF)){
        p->fdlist.push_back(parsefundec());
    }
    return p;
}


Fundec* Parser::parsefundec(){
    Fundec* fd = new Fundec();
    match(Token::DEF);
    match(Token::ID);
    fd->tipo = previous->text;
    match(Token::ID);
    fd->nombre = previous->text;
    match(Token::LPAREN);
    if(check(Token::ID)){
        match(Token::ID);
        fd->tipos_parametros.push_back(previous->text);
        match(Token::ID);
        fd->id_parametros.push_back(previous->text);
        while(match(Token::COMA)){
            match(Token::ID);
            fd->tipos_parametros.push_back(previous->text);
            match(Token::ID);
            fd->id_parametros.push_back(previous->text);
        }
    
    }
    match(Token::RPAREN);
    match(Token::DOSPUNTOS);
    fd->cuerpo = parseBody();
    match(Token::ENDFUN);
    return fd;
}


Body* Parser::parseBody() {
    Body* cuerpo =  new Body();
    while(check(Token::VAR)){
        cuerpo->vdlist.push_back(parsevardec());
        match(Token::SEMICOL);
    }
    cuerpo->slist.push_back(parsestmt());
    while (match(Token::SEMICOL)) {
        cuerpo->slist.push_back(parsestmt());
    }
    return cuerpo;
}


Vardec* Parser::parsevardec(){
    match(Token::VAR);
    Vardec* vd = new Vardec();
    match(Token::ID);
    vd->tipo = previous->text;
    match(Token::ID);
    vd->variables.push_back(previous->text);
    while(match(Token::COMA)){
        match(Token::ID);
        vd->variables.push_back(previous->text);
    }
    return vd; 
}

Stmt *Parser::parsestmt() {
    Exp* e;
    if (match(Token::BREAK)){
        return new BreakStmt();
    }
    if (match(Token::CONTINUE)){
        return new ContinueStmt();
    }
    if (match(Token::PRINT)) {
        match(Token::LPAREN);
        e = parseCEXP();
        match(Token::RPAREN);
        return new PrintStmt(e);
    }
    if (match(Token::RETURN)) {
        if(match(Token::SEMICOL)){
            ReturnStm* ret = new ReturnStm();
            ret->exp = nullptr;
            return ret;
        }
        ReturnStm* ret = new ReturnStm();
        ret->exp = parseCEXP(); 
        return ret;
    }
    else if (match(Token::IF)){
        e = parseCEXP();
        IfStmt* ifstm = new IfStmt(e); 
        match(Token::THEN);
        ifstm -> cuerpo_if = parseBody();
        if(match(Token::ELSE)){
            ifstm -> hayelse = true;
            ifstm -> cuerpo_else = parseBody();
        }
        match(Token::ENDIF);
        return ifstm;
    }
    
    else if (match(Token::WHILE)){
        e = parseCEXP();
        WhileStmt* Trampolin = new WhileStmt(e); 
        match(Token::DO);
        Trampolin->cuerpo = parseBody();
        match(Token::ENDWHILE);
        return Trampolin;
    }
    else if (match(Token::FOR)){
        ForStmt* forstm = new ForStmt();
        match(Token::ID);
        forstm->variable = previous->text;
        match(Token::ASSIGN);
        forstm->inicio = parseCEXP();
        match(Token::TO);
        forstm->fin = parseCEXP();
        match(Token::DO);
        forstm->cuerpo = parseBody();
        match(Token::ENDFOR);
        return forstm;
    }
        
    else if (match(Token::ID)) {
        string texto = previous->text;
        match(Token::ASSIGN);
        e = parseCEXP();
        return new AsignStmt(texto,e);
    }
}

Exp* Parser::parseCEXP() {
    Exp* l = parseAEXP();
    while (match(Token::EQUIV)|| match(Token::LET)) {
        BinaryOp op;
        if (previous->type == Token::EQUIV){
            op = EQUIV_OP;
        }
        else{
            op = LET_OP;
        }
        Exp* r = parseAEXP();
        l = new BinaryExp(l, r, op);
    }
    return l;
}
Exp* Parser::parseAEXP() {
    Exp* l = parseE();
    while (match(Token::PLUS) || match(Token::MINUS)) {
        BinaryOp op;
        if (previous->type == Token::PLUS){
            op = PLUS_OP;
        }
        else{
            op = MINUS_OP;
        }
        Exp* r = parseE();
        l = new BinaryExp(l, r, op);
    }
    return l;
}



Exp* Parser::parseE() {
    Exp* l = parseT();
    while (match(Token::MUL) || match(Token::DIV)) {
        BinaryOp op;
        if (previous->type == Token::MUL){
            op = MUL_OP;
        }
        else{
            op = DIV_OP;
        }
        Exp* r = parseT();
        l = new BinaryExp(l, r, op);
    }
    return l;
}


Exp* Parser::parseT() {
    Exp* l = parseF();
    if (match(Token::POW)) {
        BinaryOp op = POW_OP;
        Exp* r = parseF();
        l = new BinaryExp(l, r, op);
    }
    return l;
}

Exp* Parser::parseF() {
    Exp* e; 
    if (match(Token::NUM)) {
        return new NumberExp(stoi(previous->text));
    }
    else if (match(Token::ID)) {
        string variable = previous ->text;
        if(match(Token::LPAREN)){
            FcallExp* fcall = new FcallExp();
            fcall -> nombre = variable;
            fcall -> argumentos.push_back(parseCEXP());
            while(match(Token::COMA)){
                fcall -> argumentos.push_back(parseCEXP());
            }
            match(Token::RPAREN);
            return fcall;
        }
        else{ return new IdExp(variable);}
    }
    else if (match(Token::LPAREN))
    {
        e = parseCEXP();
        match(Token::RPAREN);
        return e;
    }
    else if (match(Token::SQRT))
    {   
        match(Token::LPAREN);
        e = parseCEXP();
        match(Token::RPAREN);
        return new SqrtExp(e);
    }
    else {
        throw runtime_error("Error sintáctico");
    }
}
