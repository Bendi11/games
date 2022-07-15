#pragma once

#include "bobj.h"

typedef struct blist_t blist_t;
typedef struct blist_c {
    bobj_c super;
    bobj_t* (*at)(blist_t *,size_t);
    size_t (*len)(blist_t*);
} blist_c;

typedef struct blist_t {
    bobj_t super;
} blist_t;


inline bobj_t* blist_at(blist_t* list, size_t i) { return vft_cast(blist_c, list)->at(list, i); }
inline size_t blist_len(blist_t* list) { return vft_cast(blist_c, list)->len(list); }
extern blist_c* (*blist_c_impl)(void);


