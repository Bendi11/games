#pragma once

#include "bobj.h"
#include "bobj/iter.h"
#include "bobj/fn.h"

typedef struct biter_map_t biter_map_t;
typedef struct biter_map_c {
    biter_c super;
} biter_map_c;

typedef struct biter_map_t {
    biter_t super;
    biter_t *iter;
    bfn_t *map;
} biter_map_t;

extern biter_map_c* (*biter_map_c_impl)(void);
void biter_map_new(biter_map_t *self, bfn_t* map, biter_t *iter);
biter_map_t biter_map(biter_t* iter, bfn_t* mapper);
