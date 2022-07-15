#pragma once

#include "bobj.h"

#include <stdbool.h>


typedef struct bfn_t bfn_t;
typedef struct bfn_c {
    bobj_c super;
    bool (*typecheck)(bfn_t *self, size_t arg_count, bobj_t *args[]);
    bobj_t* (*call)(bfn_t *self, size_t arg_count, bobj_t *args[]);
} bfn_c;

/** 
 * \brief Function object containing methods for calling with different argument counts and types
 */
typedef struct bfn_t {
    bobj_t super;
} bfn_t;


bobj_t *bfn_call(bfn_t *fn, size_t args_len, bobj_t *args[]);

extern bfn_c* (*bfn_c_impl)(void);

typedef struct bbinaryfn_t bbinaryfn_t;
typedef struct bbinaryfn_c {
    bfn_c super;
    bool (*typecheck)(bbinaryfn_t *self, size_t arg_count, bobj_t *args[]);
    bobj_t* (*call)(bbinaryfn_t *self, bobj_t *, bobj_t*);
} bbinaryfn_c;

typedef struct bbinaryfn_t {
    bfn_t super;
} bbinaryfn_t;
