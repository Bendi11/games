#pragma once

#include "bobj.h"
#include "bobj/list.h"

#include <stdbool.h>


typedef struct bfn_t bfn_t;
/** \extends bobj_c */
typedef struct bfn_c {
    bobj_c super;
    bool (*typecheck)(bfn_t *self, blist_t *args);
    void (*call)(bfn_t *self, blist_t *args, bobj_t* res);
} bfn_c;

/**
 * \extends bobj_t
 * \brief Function object containing methods for calling with different argument counts and types
 */
typedef struct bfn_t {
    bobj_t super;
    bobj_c *return_ty;
} bfn_t;

bobj_c* bfn_return_ty(bfn_t *fn);
void bfn_call(bfn_t *fn, blist_t *args, bobj_t* res);
bool bfn_typecheck(bfn_t *fn, blist_t *args);

extern bfn_c* (*bfn_c_impl)(void);

typedef struct bfnptr_fn_t bfnptr_fn_t;
/**
 * \extends bfn_c
 * Class type that contains a function pointer to call
 */
typedef struct bfnptr_fn_c {
    bfn_c super;
} bfnptr_fn_c;

extern bfnptr_fn_c* (*bfnptr_fn_c_impl)(void);

typedef void (*bfnptr_fn_call_t)(blist_t*, bobj_t*);
typedef bool (*bfnptr_fn_check_t)(blist_t*);

/**
 * \extends bfn_t
 * Implementation of bfn_t that contains a function pointer to call and typechecking function
 */
typedef struct bfnptr_fn_t {
    bfn_t super;
    bfnptr_fn_call_t fn;
    /** \brief List of argument types that the function should take, contains instances of bobj_class_t */
    blist_t *check;
} bfnptr_fn_t;

typedef void (*bfnptr_unary_fn_t)(bobj_t*, bobj_t*);

void bfnptr_fn_new(bfnptr_fn_t *fn, bfnptr_fn_call_t ptr, blist_t *args, bobj_c* return_ty);
bfnptr_fn_t s_bfnptr_fn(bfnptr_fn_call_t ptr, blist_t *args, bobj_c* return_ty);
bfnptr_fn_t* h_bfnptr_fn(bfnptr_fn_call_t ptr, blist_t *args, bobj_c* return_ty);
