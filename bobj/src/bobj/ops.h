#pragma once

#include "bobj.h"



/** Interface for addable objects */
typedef struct add_i {
    void (*add)(bobj_t *lhs, bobj_t *rhs, bobj_t *res);
} add_i;

/** Interface specifying that an object may be subtracted from another object */
typedef struct sub_i {
    void (*sub)(bobj_t *lhs, bobj_t *rhs, bobj_t *res);
} sub_i;

/** \brief Interface for all objects that can be divided */
typedef struct {
    void (*div)(bobj_t *lhs, bobj_t *rhs, bobj_t *res);
} div_i;

/** \brief Interface for all objects that can be multiplied */
typedef struct {
    void (*mul)(bobj_t *lhs, bobj_t *rhs, bobj_t *res);
} mul_i;
