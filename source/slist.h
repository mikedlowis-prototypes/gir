/**
  @file slist.h
*/
#ifndef SLIST_H
#define SLIST_H

#include <stdbool.h>
#include <stddef.h>

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

#endif /* SLIST_H */
