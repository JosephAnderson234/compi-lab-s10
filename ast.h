#ifndef AST_H
#define AST_H

#include <string>
#include <unordered_map>
#include <list>
#include <vector>
#include <ostream>

using namespace std;

class Visitor; 
class Body;

// Operadores binarios soportados
enum BinaryOp { 
    PLUS_OP, 
    MINUS_OP, 
    MUL_OP, 
    DIV_OP,
    LET_OP,
    EQUIV_OP,
    POW_OP
};

// Clase abstracta Exp
class Exp {
public:
    virtual int  accept(Visitor* visitor) = 0;
    virtual ~Exp() = 0;  // Destructor puro → clase abstracta
    static string binopToChar(BinaryOp op);  // Conversión operador → string
};

// Expresión binaria
class BinaryExp : public Exp {
public:
    Exp* left;
    Exp* right;
    BinaryOp op;
    int accept(Visitor* visitor);
    BinaryExp(Exp* l, Exp* r, BinaryOp op);
    ~BinaryExp();

};

// Expresión numérica
class NumberExp : public Exp {
public:
    int value;
    int accept(Visitor* visitor);
    NumberExp(int v);
    ~NumberExp();
};


class IdExp : public Exp {
public:
    string value;
    int accept(Visitor* visitor);
    IdExp(string v);
    ~IdExp();
};

// Raiz cuadrada
class SqrtExp : public Exp {
public:
    Exp* value;
    int accept(Visitor* visitor);
    SqrtExp(Exp* v);
    ~SqrtExp();
}
;

class Stmt{
public:
    virtual void accept(Visitor* visitor) = 0;
    virtual ~Stmt() = 0;
};

class AsignStmt : public Stmt {
public:
    string variable;
    Exp* exp;
    void accept(Visitor* visitor) override;
    AsignStmt(string, Exp*);
    ~AsignStmt();
};

class PrintStmt : public Stmt {
public:
    Exp* exp;
    void accept(Visitor* visitor) override;
    PrintStmt(Exp* e);
    ~PrintStmt();
};

class IfStmt : public Stmt{
public:
    Exp* condicion;
    Body* cuerpo_if;
    Body* cuerpo_else;
    bool hayelse = false;
    void accept(Visitor* visitor) override;
    IfStmt(Exp* e);
    ~IfStmt();
};

class WhileStmt : public Stmt{
public:
    Exp* condicion;
    Body* cuerpo;
    void accept(Visitor* visitor) override;
    WhileStmt(Exp* e);
    ~WhileStmt();
};

class ForStmt : public Stmt{
public:
    string variable;
    Exp* inicio;
    Exp* fin;
    Body* cuerpo;
    void accept(Visitor* visitor) override;
    ForStmt();
    ~ForStmt();
};


class BreakStmt : public Stmt{
public:
    void accept(Visitor* visitor);
    BreakStmt(){};
    ~BreakStmt(){};
};

class ContinueStmt : public Stmt{
public:
    void accept(Visitor* visitor);
    ContinueStmt(){};
    ~ContinueStmt(){};
};

class FCallStmt : public Stmt{
public:
    string nombre;
    vector<Exp*> argumentos;
    void accept(Visitor* visitor);
    FCallStmt();
    ~FCallStmt();
};

class AssigPlusStmt : public Stmt {
public:
    string variable;
    Exp* exp;
    void accept(Visitor* visitor) override;
    AssigPlusStmt(string, Exp*);
    ~AssigPlusStmt();
};

class DoWhileStmt : public Stmt{
public:
    Body* cuerpo;
    Exp* condicion;
    void accept(Visitor* visitor) override;
    DoWhileStmt();
    ~DoWhileStmt();
};


class Vardec{
public:    
    string tipo;
    list<string> variables;
    void accept(Visitor* visitor) ;
    Vardec();
    ~Vardec();
};

class Body{
public:
    list<Vardec*> vdlist;
    list<Stmt*> slist;
    void accept(Visitor* visitor);
    Body();
    ~Body();
};

class Fundec {
public:
    string nombre;
    string tipo;
    vector<string> tipos_parametros;
    vector<string> id_parametros;
    Body* cuerpo;
    void accept(Visitor* visitor);
    Fundec();
    ~Fundec();
};

class FcallExp : public Exp{
public:
    string nombre;
    vector<Exp*> argumentos;
    int accept(Visitor* visitor);
    FcallExp();
    ~FcallExp();
};

class ReturnStm : public Stmt{
public:
    Exp* exp;
    void accept(Visitor* visitor) override;
    ReturnStm();
    ~ReturnStm();
};




class Programa {
public:
    list<Vardec*> vdlist;
    list<Fundec*> fdlist;
    void accept(Visitor* visitor);
    ~Programa();
    Programa();

};

#endif // AST_H
