#pragma once

#include "bobj.h"
#include "bobj/list.h"

#include <stdbool.h>


typedef struct bfn_t bfn_t;
typedef struct bfn_c {
    bobj_c super;
    bool (*typecheck)(bfn_t *self, blist_t *args);
    bobj_t* (*call)(bfn_t *self, blist_t *args);
} bfn_c;

/** 
 * \brief Function object containing methods for calling with different argument counts and types
 */
typedef struct bfn_t {
    bobj_t super;
} bfn_t;


bobj_t *bfn_call(bfn_t *fn, blist_t *args);
inline bool bfn_typecheck(bfn_t *fn, blist_t *args) { return vft_cast(bfn_c, fn)->typecheck(fn, args); }
bfn_t *bfn_new(bobj_t* (*fn)(blist_t *));

extern bfn_c* (*bfn_c_impl)(void);

typedef struct bfnptr_fn_t bfnptr_fn_t;
typedef struct bfnptr_fn_c {
    bfn_c super;
} bfnptr_fn_c;

extern bfnptr_fn_c* (*bfnptr_fn_c_impl)(void);

typedef struct bfnptr_fn_t {
    bfn_t super;
    bobj_t* (*fn)(blist_t*);
    bool (*check)(blist_t*);
} bfnptr_fn_t;


void bfnptr_fn_new(bfnptr_fn_t *fn, bobj_t* (*fnptr)(blist_t*), bool (*check)(blist_t*));
