#pragma once

#include "bobj.h"

typedef void(*add_i_addfn)(bobj_t*,bobj_t*,bobj_t*);

/** Interface for addable objects */
typedef struct {
    btrait_t super;
    void (*add)(bobj_t *lhs, bobj_t *rhs, bobj_t *res);
} biadd_t;

extern btrait_c* (*biadd_c_impl)(void); 
void biadd_new(biadd_t *self, add_i_addfn fn);
biadd_t* h_biadd(add_i_addfn fn);

/** Interface specifying that an object may be subtracted from another object */
typedef struct {
    btrait_t super;
    void (*sub)(bobj_t *lhs, bobj_t *rhs, bobj_t *res);
} bisub_t;

/** \brief Interface for all objects that can be divided */
typedef struct {
    btrait_t super;
    void (*div)(bobj_t *lhs, bobj_t *rhs, bobj_t *res);
} bidiv_t;

/** \brief Interface for all objects that can be multiplied */
typedef struct {
    btrait_t super;
    void (*mul)(bobj_t *lhs, bobj_t *rhs, bobj_t *res);
} bimul_t;
