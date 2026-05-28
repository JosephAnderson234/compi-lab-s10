# Guía de Estudio: Compilador Didáctico (Lab Sem 10)

## Arquitectura General

```
Código fuente → Scanner → Tokens → Parser → AST → Visitors (Print / EVAL)
```

Tres fases principales:
1. **Scanner** (análisis léxico): texto → tokens
2. **Parser** (análisis sintáctico): tokens → AST (árbol)
3. **Visitors** (análisis semántico + evaluación): recorren el AST

---

## 1. Scanner (`scanner.cpp` / `token.h`)

**Función**: Convierte el texto fuente en tokens.

**Cómo funciona**:
- `nextToken()` salta whitespace, luego clasifica el primer caracter:
  - Dígito → token NUM
  - Letra → acumula alfanuméricos, compara contra keywords fijas (`if`, `while`, `def`, `print`, etc.) → token ID o keyword
  - Símbolo (`+`, `-`, `*`, `/`, `(`, `)`, `=`, `:`, etc.) → token del operador
  - Casos multi-carácter: `==` → EQUIV, `**` → POW
- Fin de entrada → token END
- Carácter inválido → token ERR

**📝 Pregunta frecuente**: *¿Dónde agregarías un nuevo keyword/token?*
→ `token.h` (enum Type), `token.cpp` (case en operator<<), `scanner.cpp` (if en la cadena de keywords)

---

## 2. Parser (`parser.cpp` / `parser.h`)

**Técnica**: Descendente recursivo manual.

**Estructura**: Una función por cada no-terminal de la gramática:

```
parseProgram  → parseBody (raíz del programa)
parseP        → (parsevardec ';')* (parsefundec)*
parsefundec   → 'def' ID ID '(' params ')' ':' parseBody 'endfun'
parseBody     → (parsevardec)* parsestmt (';' parsestmt)*
parsestmt     → break | continue | print | return | if | while | for | ID '=' exp
parseCEXP     → parseAEXP (('==' | '<') parseAEXP)?     ← comparación
parseAEXP     → parseE (('+' | '-') parseE)*            ← suma/resta (left-assoc)
parseE        → parseT (('*' | '/') parseT)*            ← multiplicación (left-assoc)
parseT        → parseF ('**' parseF)?                   ← potencia
parseF        → NUM | ID ( '(' args ')' )? | '(' exp ')' | 'sqrt' '(' exp ')'
```

**Precedencia** (de menor a mayor):
1. `==`, `<`
2. `+`, `-`
3. `*`, `/`
4. `**`
5. Factores: paréntesis, sqrt, llamadas, literales

**Asociatividad**: Se logra con bucles `while` en lugar de recursión izquierda. Si usaras recursión izquierda (`AExp → AExp '+' E`), el parser caería en **loop infinito** (stack overflow).

**Métodos auxiliares**:
- `check(ttype)`: mira el token actual sin consumirlo
- `match(ttype)`: si coincide, avanza y retorna true
- `advance()`: consume el token actual, guarda el anterior en `previous`

**🔴 Cuidado**: `advance()` hace `delete previous`. Esto es seguro porque el texto del token se copia en strings del AST. Pero el último token y el token END nunca se liberan (memoria leak menor).

---

## 3. AST (`ast.h` / `ast.cpp`)

Jerarquía de nodos:

```
Programa
├── Vardec (tipo, lista de variables)
├── Fundec (nombre, tipo retorno, parámetros, Body)
│   └── Body
│       ├── Vardec (variables locales)
│       └── Stmt
│           ├── AsignStmt (var = exp)
│           ├── PrintStmt (print(exp))
│           ├── IfStmt (cond, cuerpo_if, cuerpo_else?, hayelse bool)
│           ├── WhileStmt (cond, cuerpo)
│           ├── ForStmt (var, inicio, fin, cuerpo)
│           ├── BreakStmt / ContinueStmt
│           └── ReturnStm (exp?)
└── Exp (expresiones → devuelven int)
    ├── NumberExp (valor entero)
    ├── IdExp (nombre de variable)
    ├── BinaryExp (left, op, right)
    ├── SqrtExp (exp)
    └── FcallExp (nombre, argumentos)
```

**Patrón Visitor**: Cada nodo tiene `accept(Visitor*)` que llama a `visitor->visit(this)`. Esto permite agregar operaciones (imprimir, evaluar) sin modificar los nodos.

**🔴 Memoria leak**: El `Programa* ast` en `main.cpp` nunca se libera con `delete`. En un proyecto real, añadirías un destructor que recorra y libere todo el árbol.

---

## 4. Visitors (`visitor.h` / `visitor.cpp`)

### PrintVisitor
Recorre el AST y reconstruye el código fuente con indentación. Útil para depuración.

### EVALVisitor — El Intérprete

**Environment (`environment.h`)**:
- `vector<unordered_map<string, T>> ribs` — pila de scopes
- `add_level()` → push nuevo scope (mapa vacío)
- `remove_level()` → pop scope actual
- `add_var(name, value)` → inserta en el scope actual
- `update(name, value)` → busca en todos los scopes (del más interno al externo) y actualiza
- `lookup(name)` → busca y retorna valor

**Scoping**: Cada `Body` crea su propio scope:
```cpp
memoria.add_level();
// declaraciones de variables
// sentencias
memoria.remove_level();
```

**Valores de verdad**: 0 = falso, cualquier ≠ 0 = verdadero.

**Flujo de control**:

| Constructo | Implementación |
|---|---|
| `if` | `if(cond != 0) { if-body } else if(hayelse) { else-body }` |
| `while` | while(cond != 0) { try body; catch Break → break; catch Continue → continue } |
| `for` | add_level; for(i = inicio; i <= fin; i++) { add_var(var, i); body } remove_level |
| `break` | Lanza `BreakException` (atrapada por el while/for más cercano) |
| `continue` | Lanza `ContinueException` |
| `return` | Lanza `ReturnException(valor)` (atrapada por FcallExp o la función main) |

**Llamadas a función**:
1. Buscar `Fundec*` en `fmemoria[nombre]`
2. `add_level()` para parámetros
3. Asignar argumentos a parámetros con `add_var`
4. Ejecutar `fd->cuerpo->accept(this)` dentro de try-catch
5. Si hay `ReturnException(v)`, capturarla y guardar v
6. `remove_level()`
7. Retornar v (o 0 si no hubo return)

---

## 5. Preguntas típicas de examen / implementación

### Agregar un nuevo operador (ej: `%` módulo)
Archivos a modificar:
1. **`token.h`** — agregar `MOD` al enum `Type`
2. **`token.cpp`** — agregar case en `operator<<`
3. **`scanner.cpp`** — agregar `'%'` al `strchr` y al switch
4. **`ast.h`** — agregar `MOD_OP` al enum `BinaryOp`
5. **`ast.cpp`** — agregar case en `binopToChar`
6. **`parser.cpp`** — agregar `|| match(Token::MOD)` en `parseE`
7. **`visitor.cpp` (EVALVisitor)** — agregar case `MOD_OP` en `visit(BinaryExp)`
8. **`visitor.cpp` (PrintVisitor)** — agregar case `MOD_OP` en `visit(BinaryExp)`

### Agregar un nuevo statement (ej: `repeat ... until (cond)`)
1. **`token.h`** — agregar `REPEAT`, `UNTIL` al enum
2. **`token.cpp`** — agregar cases en `operator<<`
3. **`scanner.cpp`** — agregar `"repeat"` y `"until"` como keywords
4. **`ast.h`** — crear clase `RepeatStmt : public Stmt` con `Exp* condicion; Body* cuerpo;`
5. **`ast.cpp`** — constructor y destructor de `RepeatStmt`
6. **`parser.cpp`** — agregar `parsestmt()`: si `match(REPEAT)` → parsear Body, matchear UNTIL, parsear CEXP, matchear ENDIF/semicolon
7. **`visitor.h`** — agregar `virtual void visit(RepeatStmt*) = 0;`
8. **`visitor.cpp`** — implementar en ambos Visitors:
   - EVALVisitor: `do { try body } while(cond == 0);`
   - PrintVisitor: imprimir "repeat ... until (cond)"

### Agregar arrays
- **Vardec**: agregar `int tamaño` opcional y `bool esArray`
- **Nuevo nodo `ArrayAccessExp`**: `IdExp* nombre; Exp* indice;` (para `arr[i]`)
- **Parser**: parsear `ID '[' CEXP ']'` en `parseF`
- **EVALVisitor**: `Environment` necesitaría cambiar de `int` a `vector<int>` o crear un `Environment` paralelo. O usar un `unordered_map<string, vector<int>>` aparte.

### Inicialización explícita `var int x = 5`
- **Parser**: en `parsevardec()`, después del ID, si hay `ASSIGN`, parsear `CEXP`
- **AST**: `Vardec` necesita una lista `list<Exp*> valores_iniciales` (paralela a `variables`)
- **EVALVisitor**: en `visit(Vardec)`, evaluar `valores_iniciales[i]` y usar ese valor en lugar de `T()`

### Declaración implícita de variables
- Modificar `EVALVisitor::visit(AsignStmt)`:
  - Antes de `memoria.update()`, verificar si la variable existe con `memoria.check()`
  - Si no existe, hacer `memoria.add_var(stm->variable, stm->exp->accept(this))`
  - En lugar de `update`, usar `add_var` en el scope actual

### Análisis semántico (type checking)
Actualmente no existe. Para agregarlo:
- Crear un nuevo `SemanticVisitor` que recorra el AST antes de la evaluación
- Tabla de símbolos que almacene tipos de variables y funciones
- Verificar: variables declaradas antes de usar, tipos en asignaciones, número de argumentos en llamadas, return type vs declaración

### Casos borde y errores comunes
| Situación | ¿Qué pasa? | ¿Cómo arreglarlo? |
|---|---|---|
| Sin función `main` | **Segfault**: `fmemoria["main"]` es nullptr | Null-check antes de llamar |
| `factorial(-1)` | **Stack overflow**: recursión infinita | Condición base más robusta (`x <= 1`) |
| División por cero | Error en runtime, resultado 0 | Manejo existente con if(v2 != 0) |
| Variable no declarada | `lookup` retorna 0 con warning | Agregar análisis semántico previo |
| AST nunca se libera | Memory leak al salir | `delete ast` al final de `main` |

---

## Resumen visual del flujo

```
"var int x; x = 2 + 3; print(x)"
    │
    ▼ Scanner (nextToken)
    │
    [VAR] [ID("int")] [ID("x")] [SEMICOL]
    [ID("x")] [ASSIGN] [NUM("2")] [PLUS] [NUM("3")] [SEMICOL]
    [PRINT] [LPAREN] [ID("x")] [RPAREN] [END]
    │
    ▼ Parser (recursive descent)
    │
    Programa {
      vdlist: [Vardec { tipo: "int", variables: ["x"] }],
      fdlist: []
    }
    (el parser construye el AST llamando a parseP → parsevardec → parsestmt → parseCEXP → ...)
    │
    ▼ Visitors
    │
    PrintVisitor →  "var int x,\nx = 2 + 3\nprint(x)\n"
    │
    EVALVisitor →  "Interprete:\n5\n"
```
