/**
  @file gir_internals.h
  @brief TODO: Describe this file
  */
#ifndef GIR_INTERNALS_H
#define GIR_INTERNALS_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

/******************************************************************************
 * Garbage Collection
 *****************************************************************************/
void gc_init(void** stkbtm);
void gc_deinit(void);
void* gc_alloc(size_t sz);
void gc_addref(void* obj);
void gc_delfref(void* obj);

/******************************************************************************
 * Singly Linked List
 *****************************************************************************/
typedef struct slist_node_t {
    struct slist_node_t* next;
} slist_node_t;

typedef struct {
    slist_node_t* head;
} slist_t;

void slist_init(slist_t* list);

bool slist_empty(slist_t* list);

size_t slist_size(slist_t* list);

slist_node_t* slist_front(slist_t* list);

void slist_push_front(slist_t* list, slist_node_t* node);

slist_node_t* slist_pop_front(slist_t* list);

slist_node_t* slist_back(slist_t* list);

void slist_push_back(slist_t* list, slist_node_t* node);

slist_node_t* slist_pop_back(slist_t* list);

bool slist_node_has_next(slist_node_t* node);

slist_node_t* slist_node_next(slist_node_t* node);

#define slist_foreach(elem, list) \
    for(slist_node_t* elem = slist_front(list); elem != NULL; elem = elem->next)

/******************************************************************************
 * Hash Array Mapped Trie
 *****************************************************************************/
typedef struct hamt_node_t {
    uintptr_t bitmap_or_key; /* bitmap or hash key */
    uintptr_t base_or_value; /* base of entry list or value */
} hamt_node_t;

typedef void (*hamt_keyfn_t)(void* entry, uint8_t** addr, size_t* len);

typedef void (*hamt_delfn_t)(void* entry);

typedef int (*hamt_cmpfn_t)(void* a, void* b);

typedef struct hamt_t {
    hamt_node_t* root;
    hamt_keyfn_t getkey;
    hamt_delfn_t delentry;
    hamt_cmpfn_t cmpentry;
} hamt_t;

typedef struct {
    uint32_t hash;
    uint32_t part;
    uint32_t bits;
    uint32_t level;
    hamt_node_t* grandparent;
    hamt_node_t* parent;
    hamt_node_t* current;
} hamt_ctx_t;

void hamt_init(hamt_t* trie, hamt_keyfn_t getkey, hamt_delfn_t delfn, hamt_cmpfn_t cmpfn);

void* hamt_lookup(hamt_t* trie, void* key);

void hamt_insert(hamt_t* trie, void* key);

/******************************************************************************
 * User-facing Opaque Types
 *****************************************************************************/
struct ObjHdr {
    uint32_t type;
    uint32_t size;
    struct Obj* parent;
    hamt_t slots;
};

struct Obj {
    struct ObjHdr hdr;
    uint8_t data[];
};

struct GirCtx {
    hamt_t intern_pool;
    struct Obj* Lobby;
    struct Obj* Nil;
    struct Obj* Bool;
    struct Obj* True;
    struct Obj* False;
    struct Obj* Num;
    struct Obj* String;
    struct Obj* Symbol;
    struct Obj* List;
    struct Obj* Array;
    struct Obj* ByteArray;
    struct Obj* Map;
    struct Obj* Set;
    struct Obj* Block;
};

struct Slot {
    uintptr_t sel;
    struct Obj* val;
};

/******************************************************************************
 * Builtin Object Types
 *****************************************************************************/
#define TYPE_BARE   0
#define TYPE_BOOL   1
#define TYPE_NUM    2
#define TYPE_STRING 3
#define TYPE_SYMBOl 4
#define TYPE_LIST   5
#define TYPE_ARRAY  6
#define TYPE_MAP    7
#define TYPE_SET    8
#define TYPE_BLOCK  9

typedef struct ObjBool {
    struct ObjHdr hdr;
    bool val;
} ObjBool;

typedef struct ObjNum {
    struct ObjHdr hdr;
    double val;
} ObjNum;

typedef struct ObjString {
    struct ObjHdr hdr;
    char* val;
} ObjString;

typedef struct ObjSymbol {
    struct ObjHdr hdr;
    uintptr_t val;
} ObjSymbol;

typedef struct ObjList {
    struct ObjHdr hdr;
    slist_t val;
} ObjList;

//typedef struct ObjArray {
//    struct ObjHdr hdr;
//    vec_t val;
//} ObjArray;

typedef struct ObjMap {
    struct ObjHdr hdr;
    hamt_t val;
} ObjMap;

typedef struct ObjSet {
    struct ObjHdr hdr;
    hamt_t val;
} ObjSet;

//typedef struct ObjBlock {
//    struct ObjHdr hdr;
//    bytecode_t val;
//} ObjBlock;

/******************************************************************************
 * Raw Object Operations
 *****************************************************************************/
struct Obj* obj_new(struct Obj* parent, uint32_t type, size_t datasz);
void obj_set(struct Obj* obj, uintptr_t sel, struct Obj* val);
struct Obj* obj_get(struct Obj* obj, uintptr_t sel);

#endif /* GIR_INTERNALS_H */
