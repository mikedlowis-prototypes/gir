#include "gir.h"
#include "gir_internals.h"

/* String Interning
 *****************************************************************************/
static uintptr_t intern(GirCtx* ctx, char* str) {
    uintptr_t sym = (uintptr_t)hamt_lookup(&(ctx->intern_pool), str);
    if (sym == 0) {
        hamt_insert(&(ctx->intern_pool), str);
        sym = (uintptr_t)str;
    }
    return sym;
}

static void intern_key(void* entry, uint8_t** addr, size_t* len) {
    *addr = (uint8_t*)entry;
    *len = strlen((char*)entry);
}

static void intern_del(void* entry) {
    (void)entry; // noop
}

static int intern_cmp(void* a, void* b) {
    return strcmp((char*)a, (char*)b);
}

/* Slot Management
 *****************************************************************************/
static Slot* slot_new(uintptr_t sel, Obj* val) {
    (void)sel;
    (void)val;
    return NULL;
}

static void slot_key(void* entry, uint8_t** addr, size_t* len) {
    *addr = (uint8_t*)&(((Slot*)entry)->sel);
    *len  = sizeof(uintptr_t);
}

static void slot_del(void* entry) {
    free(entry);
}

static int slot_cmp(void* a, void* b) {
    Slot* sa = (Slot*)a;
    Slot* sb = (Slot*)b;
    return sa->sel - sb->sel;
}

/* Base Object Definition
 *****************************************************************************/
Obj* mkobject(uint32_t type, size_t datasz) {
    Obj* obj    = (Obj*)malloc(sizeof(struct Obj) + datasz);
    obj->type   = type;
    obj->size   = datasz;
    obj->parent = NULL;
    hamt_init(&(obj->slots), &slot_key, &slot_del, &slot_cmp);
    return obj;
}

/* Main API
 *****************************************************************************/
GirCtx* gir_init(void)
{
    GirCtx* ctx = malloc(sizeof(struct GirCtx));
    hamt_init(&(ctx->intern_pool), &intern_key, &intern_del, &intern_cmp);
    ctx->lobby = NULL;
    return ctx;
}

void gir_evalfile(FILE* input, const char* prompt)
{
    (void)input;
    (void)prompt;
}

void gir_deinit(GirCtx* ctx)
{
    free(ctx);
}











//Obj* Lobby;
//Obj* Nil;
//Obj* Bool;
//Obj* True;
//Obj* False;
//Obj* Num;
//Obj* String;
//Obj* Symbol;
//Obj* List;
//Obj* Array;
//Obj* ByteArray;
//Obj* Map;
//Obj* Set;
//Obj* Block;
//
//hamt_t InternPool;
//
///* Private API Declarations
// *****************************************************************************/
//
///* Top-Level API
// *****************************************************************************/
//void gir_init(void* stack_btm)
//{
//    (void)stack_btm;
////    hamt_init(&InternPool);
//}
//
//void gir_deinit(void)
//{
//}
//
//Obj* gir_evalexpr(FILE* input)
//{
//    (void)input;
//    return Nil;
//}
//
//void gir_evalfile(FILE* input, const char* prompt)
//{
//    extern void exec_file(FILE* file, const char* prompt);
//    exec_file(input, prompt);
//}
//
//Obj* gir_send(Obj* rcvr, const char* sel, ...)
//{
//    (void)rcvr;
//    (void)sel;
//    return Nil;
//}
//
///* Constructors
// *****************************************************************************/
//Obj* gir_mkobject(double val)
//{
//    (void)val;
//    return Nil;
//}
//
//Obj* gir_mknum(double val)
//{
//    (void)val;
//    return Nil;
//}
//
//Obj* gir_mkstring(const char* val)
//{
//    (void)val;
//    return Nil;
//}
//
//Obj* gir_mksymbol(const char* val)
//{
//    (void)val;
//    return Nil;
//}
//
//Obj* gir_mklist(size_t num, ...)
//{
//    (void)num;
//    return Nil;
//}
//
//Obj* gir_mkarray(size_t num, ...)
//{
//    (void)num;
//    return Nil;
//}
//
//Obj* gir_mkbytearray(size_t num, uint8_t* val)
//{
//    (void)num;
//    (void)val;
//    return Nil;
//}
//
//Obj* gir_mkmap(void)
//{
//    return Nil;
//}
//
//Obj* gir_mkset(void)
//{
//    return Nil;
//}
//
//Obj* gir_mkblock(void)
//{
//    return Nil;
//}

/* Private API Definitions
 *****************************************************************************/

