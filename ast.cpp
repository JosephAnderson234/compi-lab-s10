#include "ast.h"
#include <iostream>

using namespace std;

// ------------------ Exp ------------------
Exp::~Exp() {}

string Exp::binopToChar(BinaryOp op) {
    switch (op) {
        case PLUS_OP:  return "+";
        case MINUS_OP: return "-";
        case MUL_OP:   return "*";
        case DIV_OP:   return "/";
        case LET_OP:   return "<";
        case POW_OP:   return "**";
        case EQUIV_OP:   return "=";
        case LORT_OP:   return "<=";
        case AND_OP:   return "and";
        case OR_OP:   return "or";
        default:       return "?";
    }
}

string Exp::unopToChar(UnaryOp op) {
    switch (op) {
        case NOT_OP:   return "not";
        default:       return "?";
    }
}

// ------------------ BinaryExp ------------------
BinaryExp::BinaryExp(Exp* l, Exp* r, BinaryOp o)
    : left(l), right(r), op(o) {}

    
BinaryExp::~BinaryExp() {
    delete left;
    delete right;
}



// ------------------ NumberExp ------------------
NumberExp::NumberExp(int v) : value(v) {}

NumberExp::~NumberExp() {}


// ------------------ SqrtExp ------------------
SqrtExp::SqrtExp(Exp* v) : value(v) {}

SqrtExp::~SqrtExp() {}

//
Programa::Programa() {}
Programa::~Programa(){}

Stmt::~Stmt(){}

PrintStmt::PrintStmt(Exp* e) {
    exp=e;
}

PrintStmt::~PrintStmt() {

}

AsignStmt::AsignStmt(string texto, Exp * e) {
    variable=  texto;
    exp = e;
}

AsignStmt::~AsignStmt() {

}

FCallStmt::FCallStmt(){}
FCallStmt::~FCallStmt(){}

AssigPlusStmt::AssigPlusStmt(string texto, Exp * e) {
    variable=  texto;
    exp = e;
}

AssigPlusStmt::~AssigPlusStmt() {
    delete exp;
}

DoWhileStmt::~DoWhileStmt() {
    delete cuerpo;
    delete condicion;
}
DoWhileStmt::DoWhileStmt() {}

// ------------------ NumberExp ------------------
IdExp::IdExp(string v) : value(v) {}

IdExp::~IdExp() {}


IfStmt::IfStmt(Exp* e){
    condicion = e;
}

IfStmt::~IfStmt(){}

WhileStmt::WhileStmt(Exp* e){
    condicion = e;
}

WhileStmt::~WhileStmt(){}
Vardec::Vardec(){}
Vardec::~Vardec(){}

Body::Body(){}
Body::~Body(){}

Fundec::Fundec(){}
Fundec::~Fundec(){}

ReturnStm::ReturnStm(){}
ReturnStm::~ReturnStm(){}

FcallExp::FcallExp(){}
FcallExp::~FcallExp(){}

ForStmt::ForStmt(){}
ForStmt::~ForStmt(){}