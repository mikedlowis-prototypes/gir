#ifndef GIR_H
#define GIR_H

#include <stdio.h>
#include <stdint.h>

typedef struct Obj Obj;
typedef struct GirCtx GirCtx;
typedef struct Slot Slot;

/******************************************************************************
 *
 *****************************************************************************/


//*/
//extern Obj* Lobby;
//extern Obj* Nil;
//extern Obj* Bool;
//extern Obj* True;
//extern Obj* False;
//extern Obj* Num;
//extern Obj* String;
//extern Obj* Symbol;
//extern Obj* List;
//extern Obj* Array;
//extern Obj* ByteArray;
//extern Obj* Map;
//extern Obj* Set;
//extern Obj* Block;
/******************************************************************************
 * Top Level API
 *****************************************************************************/
GirCtx* gir_init(void);
void gir_evalfile(FILE* input, const char* prompt);
void gir_deinit(GirCtx* ctx);











//Obj* gir_evalexpr(FILE* input);
//Obj* gir_send(Obj* rcvr, const char* sel, ...);
//
///*
//Lobby
//    Nil
//    Bool
//        True
//        False
//    Num
//    String
//    Symbol
//    List
//    Array
//    ByteArray
//    Map
//    Set
//    Block
//*/
//extern Obj* Lobby;
//extern Obj* Nil;
//extern Obj* Bool;
//extern Obj* True;
//extern Obj* False;
//extern Obj* Num;
//extern Obj* String;
//extern Obj* Symbol;
//extern Obj* List;
//extern Obj* Array;
//extern Obj* ByteArray;
//extern Obj* Map;
//extern Obj* Set;
//extern Obj* Block;
//
//Obj* gir_mknum(double val);
//Obj* gir_mkstring(const char* val);
//Obj* gir_mksymbol(const char* val);
//Obj* gir_mklist(size_t num, ...);
//Obj* gir_mkarray(size_t num, ...);
//Obj* gir_mkbytearray(size_t num, uint8_t* val);
//Obj* gir_mkmap(void);
//Obj* gir_mkset(void);
//Obj* gir_mkblock(void);

#endif /* GIR_H */
