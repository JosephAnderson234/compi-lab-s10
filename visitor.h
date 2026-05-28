#ifndef VISITOR_H
#define VISITOR_H
#include "ast.h"
#include "environment.h"
#include <list>

#include <unordered_map>

class BinaryExp;
class NumberExp;
class SqrtExp;

class Visitor {
public:
    virtual int visit(BinaryExp* exp) = 0;
    virtual int visit(NumberExp* exp) = 0;
    virtual int visit(IdExp* exp) = 0;
    virtual int visit(SqrtExp* exp) = 0;
    virtual void visit(AsignStmt* stm) = 0;
    virtual void visit(PrintStmt* stm) = 0;
    virtual void visit(IfStmt* stm) = 0;
    virtual void visit(WhileStmt* stm) = 0;
    virtual void visit(ForStmt* stm) = 0;
    virtual void visit(Vardec* vd) = 0;
    virtual void visit(Body* cuerpo) = 0;
    virtual int visit(FcallExp* exp) = 0;
    virtual void visit(ReturnStm* stm) = 0;
    virtual void visit(BreakStmt * stm) = 0;
    virtual void visit(ContinueStmt * stm) = 0;
    virtual void visit(Fundec* fd) = 0;
    virtual int visit(UnaryExp* exp) = 0;
    virtual void visit(FCallStmt* stm) = 0;
    virtual void visit(Programa* program) = 0;
    virtual void visit(AssigPlusStmt* stm) = 0;
    virtual void visit(DoWhileStmt* stm) = 0;
};

class PrintVisitor : public Visitor {
public:
    int nivel = 0;

    void imprimir_indentacion() {
        for (int i = 0; i < nivel; i++) {
            cout << "    ";
        }
    }
    int visit(BinaryExp* exp) override;
    int visit(NumberExp* exp) override;
    int visit(SqrtExp* exp) override;
    void visit(AsignStmt* stm) override;
    void visit(PrintStmt* stm) override;
    void visit(ContinueStmt * stm) override;
    void visit(Programa* program) override;
    int visit(IdExp* exp) override;
    void visit(IfStmt* stm) override;
    void visit(WhileStmt* stm) override;
    int visit(UnaryExp* exp) override;
    void visit(ForStmt* stm) override;
    void visit(Vardec* vd) ;
    int  visit(FcallExp* exp) ;
    void visit(ReturnStm* stm) ;
    void visit(BreakStmt * stm);
    void visit(Fundec* fd) ;
    void visit(Body* cuerpo);
    void imprimir(Programa* program);
    void visit(FCallStmt* stm) override;
    void visit(AssigPlusStmt* stm) override;
    void visit(DoWhileStmt* stm) override;
};

class EVALVisitor : public Visitor {
public:
    Environment<int> memoria; 
    unordered_map<string,Fundec*> fmemoria;
    int retorno;
    int visit(BinaryExp* exp) override;
    int visit(NumberExp* exp) override;
    int visit(SqrtExp* exp) override;
    void visit(AsignStmt* stm) override;
    void visit(PrintStmt* stm) override;
    int visit(IdExp* exp) override;
    void visit(IfStmt* stm) override;
    void visit(WhileStmt* stm) override;
    void visit(ForStmt* stm) override;
    void visit(Programa* program) override;
    int visit(UnaryExp* exp) override;
    void visit(Vardec* vd);
    void visit(Body* cuerpo);
    void visit(BreakStmt * stm);
    void visit(ContinueStmt * stm);
    int  visit(FcallExp* exp) ;
    void visit(ReturnStm* stm) ;
    void visit(Fundec* fd) ;
    void interprete(Programa* program);
    void visit(FCallStmt* stm) override;
    void visit(AssigPlusStmt* stm) override;
    void visit(DoWhileStmt* stm) override;
};


#endif // VISITOR_H