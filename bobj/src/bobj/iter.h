#pragma once

#include "bobj.h"
#include "bobj/fn.h"
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

inline bobj_t *biter_next(biter_t *self) { return vft_cast(biter_c, self)->next(self); }

extern biter_c* (*biter_c_impl)(void);

bobj_t* biter_fold(biter_t *self, bobj_t *acc, bfn_t *fold);


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
void blist_iter_new(blist_iter_t *self, blist_t *list);
blist_iter_t s_blist_iter(blist_t *list);
blist_iter_t* h_blist_iter(blist_t *list);
