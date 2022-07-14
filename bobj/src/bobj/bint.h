#pragma once

#include "bobj.h"
#include "bobj/ops.h"

typedef struct bint_v {
    bobj_v super;
    add_i add;
    sub_i sub;
    mul_i mul;
    div_i div;
} bint_v;

/** \brief Get the default vtable for the bint_t type */
extern bint_v* (*bint_v_impl)(void);

typedef struct bint_t {
    bobj_t super;
    int val;
} bint_t;

/** \brief Initialize an instance of class `bint` */
void bint_new(bint_t *, int val);
