#pragma once

#include "bobj.h"
#include "bobj/fn.h"

/**
 * \extends btrait_t
 * Interface for addable objects 
 */
typedef struct {
    btrait_t super;
    bfn_t *add;
} biadd_t;

extern btrait_c* (*biadd_c_impl)(void); 
void biadd_new(biadd_t *self, bfn_t *fn);
biadd_t* h_biadd(bfn_t *fn);

/**
 * \extends btrait_t
 * Interface specifying that an object may be subtracted from another object
 */
typedef struct {
    btrait_t super;
    bfn_t *sub;
} bisub_t;

extern btrait_c* (*bisub_c_impl)(void); 
void bisub_new(bisub_t *self, bfn_t *fn);
bisub_t* h_bisub(bfn_t *fn);


/**
 * \extends btrait_t
 * \brief Interface for all objects that can be divided
 */
typedef struct {
    btrait_t super;
    bfn_t *div;
} bidiv_t;

extern btrait_c* (*bidiv_c_impl)(void); 
void bidiv_new(bidiv_t *self, bfn_t *fn);
bidiv_t* h_bidiv(bfn_t *fn);


/**
 * \extends btrait_t
 * \brief Interface for all objects that can be multiplied
 */
typedef struct {
    btrait_t super;
    bfn_t *mul;
} bimul_t;

extern btrait_c* (*bimul_c_impl)(void); 
void bimul_new(bimul_t *self, bfn_t *fn);
bimul_t* h_bimul(bfn_t *fn);

/**
 * \extends btrait_t
 * Interface for all objects that may be compared for equality
 */
typedef struct {
    btrait_t super;
    /** Signature (bobj_t*, bobj_t*) -> bbool_t */
    bfn_t *eq;
} bieq_t;

extern btrait_c* (*bieq_c_impl)(void);
void bieq_new(bieq_t *self, bfn_t *fn);
bieq_t* h_bieq(bfn_t *eq);

/** \brief Check if the two bobj_t's are equal */
bool bieq_eq(bobj_t *lhs, bobj_t *rhs);
