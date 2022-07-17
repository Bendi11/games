#pragma once

#include "bobj.h"

typedef void(*add_i_addfn)(bobj_t*,bobj_t*,bobj_t*);

/** Interface for addable objects */
typedef struct add_i_t {
    btrait_t super;
    void (*add)(bobj_t *lhs, bobj_t *rhs, bobj_t *res);
} add_i_t;

extern btrait_c* (*add_i_c_impl)(void); 
void add_i_new(add_i_t *self, add_i_addfn fn);
add_i_t* h_add_i(add_i_addfn fn);

/** Interface specifying that an object may be subtracted from another object */
typedef struct sub_i_t {
    btrait_t super;
    void (*sub)(bobj_t *lhs, bobj_t *rhs, bobj_t *res);
} sub_i_t;

/** \brief Interface for all objects that can be divided */
typedef struct div_i_t {
    btrait_t super;
    void (*div)(bobj_t *lhs, bobj_t *rhs, bobj_t *res);
} div_i_t;

/** \brief Interface for all objects that can be multiplied */
typedef struct mul_i_t {
    btrait_t super;
    void (*mul)(bobj_t *lhs, bobj_t *rhs, bobj_t *res);
} mul_i_t;
