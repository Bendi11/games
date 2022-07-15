#pragma once

#include "bobj.h"
#include "bobj/list.h"

typedef struct biter_t biter_t;
typedef struct biter_c {
    bobj_c super;
    bobj_t* (*next)(biter_t* self);
} biter_c;

/**
 * Class that all iterators extend, allowing algorithms operating on
 * lists and series to be written in a polymorphic way
 */
typedef struct biter_t {
    bobj_t super;
} biter_t;

extern biter_c* (*biter_c_impl)(void);

typedef struct blist_iter_t blist_iter_t;
typedef struct blist_iter_c {
    biter_c super;
} blist_iter_c;

typedef struct blist_iter_t {
    biter_t super;
    blist_t *list;
    size_t i;
} blist_iter_t;

extern blist_iter_c* (*blist_iter_c_impl)(void);
