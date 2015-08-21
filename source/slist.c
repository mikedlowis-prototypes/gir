#include "slist.h"

void slist_init(slist_t* list)
{
    list->head = NULL;
}

bool slist_empty(slist_t* list)
{
    return (list->head == NULL);
}

size_t slist_size(slist_t* list)
{
    size_t sz = 0;
    slist_node_t* node = list->head;
    while (node != NULL) {
        sz++;
        node = node->next;
    }
    return sz;
}

slist_node_t* slist_front(slist_t* list)
{
    return list->head;
}

void slist_push_front(slist_t* list, slist_node_t* node)
{
    node->next = list->head;
    list->head = node;
}

slist_node_t* slist_pop_front(slist_t* list)
{
    slist_node_t* node = list->head;
    list->head = node->next;
    node->next = NULL;
    return node;
}

slist_node_t* slist_back(slist_t* list)
{
    slist_node_t* node = list->head;
    while (node && node->next != NULL)
        node = node->next;
    return (node) ? node : NULL;
}

void slist_push_back(slist_t* list, slist_node_t* node)
{
    slist_node_t* back = slist_back(list);
    if (NULL != back)
        back->next = node;
    else
        list->head = node;
    node->next = NULL;
}

slist_node_t* slist_pop_back(slist_t* list)
{
    slist_node_t* prev = NULL;
    slist_node_t* curr = list->head;
    while (curr && curr->next != NULL) {
        prev = curr;
        curr = curr->next;
    }
    if (prev != NULL)
        prev->next = NULL;
    if (list->head == curr)
        list->head = NULL;
    return curr;
}

bool slist_node_has_next(slist_node_t* node)
{
    return (node->next != NULL);
}

slist_node_t* slist_node_next(slist_node_t* node)
{
    return node->next;
}

