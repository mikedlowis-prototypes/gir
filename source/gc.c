#include "gir_internals.h"

void gc_init(void** stkbtm) {
    (void)stkbtm;
}

void gc_deinit(void) {
}

void* gc_alloc(size_t sz) {
    (void)sz;
    return NULL;
}

void gc_addref(void* obj) {
    (void)obj;
}

void gc_delfref(void* obj) {
    (void)obj;
}
