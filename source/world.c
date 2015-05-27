/**
  @file world.c.c
  @brief See header for details
  */
//#include "world.h"

#include "map.h"

typedef struct {
    map_t* parent;
    map_t* self;
    uint8_t data[];
} object_t;

void object_free(void* obj)
{
    object_t* object = (object_t*)obj;
    if (object->parent)
        mem_release(object->parent);
    if (object->self)
        mem_release(object->self);
}

object_t* object_clone(object_t* parent)
{
    object_t* obj = (object_t*)mem_allocate(sizeof(object_t), &object_free);
    obj->parent = (NULL == parent) ? NULL : mem_retain(parent->self);
    obj->self   = map_new(NULL, NULL);
    return obj;
}


