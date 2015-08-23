/**
  @file world.c.c
  @brief See header for details
  */
//#include "world.h"

#if 0
#include "set.h"
#include "map.h"

typedef struct object_t {
    struct object_t* parent;
    map_t* slots;
    uint8_t value[];
} object_t;

set_t* Selectors = NULL;
object_t* Lobby = NULL;
object_t* Object = NULL;
object_t* Number = NULL;
object_t* String = NULL;
object_t* Array = NULL;
object_t* ByteArray = NULL;
object_t* HashSet = NULL;
object_t* HashMap = NULL;

void object_free(void* obj)
{
    object_t* object = (object_t*)obj;
    if (object->parent)
        mem_release(object->parent);
    if (object->slots)
        mem_release(object->slots);
}

object_t* object_clone(object_t* parent)
{
    object_t* obj = (object_t*)mem_allocate(sizeof(object_t), &object_free);
    if (NULL != parent)
        mem_retain(parent);
    obj->parent = parent;
    obj->slots  = map_new(NULL, NULL);
    return obj;
}


void world_init(void)
{
    Selectors = set_new(NULL, NULL);
    Lobby     = object_clone(NULL);
    Object    = object_clone(Lobby);
    Number    = object_clone(Object);
    String    = object_clone(Object);
    Array     = object_clone(Object);
    ByteArray = object_clone(Object);
    HashSet   = object_clone(Object);
    HashMap   = object_clone(Object);
}
#endif

void world_init(void) {
}

