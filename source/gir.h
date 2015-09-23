/**
  @file gir.h
*/
#ifndef GIR_H
#define GIR_H

#include <stdio.h>
#include <stdint.h>

typedef struct Obj Obj;

void gir_init(void* stack_btm);
void gir_deinit(void);
Obj* gir_evalexpr(FILE* input);
void gir_evalfile(FILE* input, const char* prompt);
Obj* gir_send(Obj* rcvr, const char* sel, ...);

/*
Lobby
    Nil
    Bool
        True
        False
    Num
    String
    Symbol
    List
    Array
    ByteArray
    Map
    Set
    Block
*/
extern Obj* Lobby;
extern Obj* Nil;
extern Obj* Bool;
extern Obj* True;
extern Obj* False;
extern Obj* Num;
extern Obj* String;
extern Obj* Symbol;
extern Obj* List;
extern Obj* Array;
extern Obj* ByteArray;
extern Obj* Map;
extern Obj* Set;
extern Obj* Block;

Obj* gir_mknum(double val);
Obj* gir_mkstring(const char* val);
Obj* gir_mksymbol(const char* val);
Obj* gir_mklist(size_t num, ...);
Obj* gir_mkarray(size_t num, ...);
Obj* gir_mkbytearray(size_t num, uint8_t* val);
Obj* gir_mkmap(void);
Obj* gir_mkset(void);
Obj* gir_mkblock(void);

#endif /* GIR_H */
