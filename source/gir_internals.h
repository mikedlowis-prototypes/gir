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

#endif /* GIR_INTERNALS_H */
