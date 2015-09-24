#include "gir.h"
#include "gir_internals.h"

struct Obj {
    uint32_t type;
    uint8_t data[];
};

Obj* Lobby;
Obj* Nil;
Obj* Bool;
Obj* True;
Obj* False;
Obj* Num;
Obj* String;
Obj* Symbol;
Obj* List;
Obj* Array;
Obj* ByteArray;
Obj* Map;
Obj* Set;
Obj* Block;

hamt_t InternPool;

/* Private API Declarations
 *****************************************************************************/

/* Top-Level API
 *****************************************************************************/
void gir_init(void* stack_btm)
{
    (void)stack_btm;
//    hamt_init(&InternPool);
}

void gir_deinit(void)
{
}

Obj* gir_evalexpr(FILE* input)
{
    (void)input;
    return Nil;
}

void gir_evalfile(FILE* input, const char* prompt)
{
    extern void exec_file(FILE* file, const char* prompt);
    exec_file(input, prompt);
}

Obj* gir_send(Obj* rcvr, const char* sel, ...)
{
    (void)rcvr;
    (void)sel;
    return Nil;
}


/* Constructors
 *****************************************************************************/
Obj* gir_mknum(double val)
{
    (void)val;
    return Nil;
}

Obj* gir_mkstring(const char* val)
{
    (void)val;
    return Nil;
}

Obj* gir_mksymbol(const char* val)
{
    (void)val;
    return Nil;
}

Obj* gir_mklist(size_t num, ...)
{
    (void)num;
    return Nil;
}

Obj* gir_mkarray(size_t num, ...)
{
    (void)num;
    return Nil;
}

Obj* gir_mkbytearray(size_t num, uint8_t* val)
{
    (void)num;
    (void)val;
    return Nil;
}

Obj* gir_mkmap(void)
{
    return Nil;
}

Obj* gir_mkset(void)
{
    return Nil;
}

Obj* gir_mkblock(void)
{
    return Nil;
}

/* Private API Definitions
 *****************************************************************************/

