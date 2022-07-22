#pragma once

#include "bobj.h"
#include "bobj/fn.h"

/** Interface for addable objects */
typedef struct {
    btrait_t super;
    bfn_t *add;
} biadd_t;

extern btrait_c* (*biadd_c_impl)(void); 
void biadd_new(biadd_t *self, bfn_t *fn);
biadd_t* h_biadd(bfn_t *fn);

/** Interface specifying that an object may be subtracted from another object */
typedef struct {
    btrait_t super;
    bfn_t *sub;
} bisub_t;

extern btrait_c* (*bisub_c_impl)(void); 
void bisub_new(bisub_t *self, bfn_t *fn);
bisub_t* h_bisub(bfn_t *fn);


/** \brief Interface for all objects that can be divided */
typedef struct {
    btrait_t super;
    bfn_t *div;
} bidiv_t;

extern btrait_c* (*bidiv_c_impl)(void); 
void bidiv_new(bidiv_t *self, bfn_t *fn);
bidiv_t* h_bidiv(bfn_t *fn);


/** \brief Interface for all objects that can be multiplied */
typedef struct {
    btrait_t super;
    bfn_t *mul;
} bimul_t;

extern btrait_c* (*bimul_c_impl)(void); 
void bimul_new(bimul_t *self, bfn_t *fn);
bimul_t* h_bimul(bfn_t *fn);

