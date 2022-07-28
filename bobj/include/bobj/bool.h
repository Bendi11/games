#pragma once

#include "bobj.h"
#include <stdbool.h>


typedef struct {
    bobj_c super;
} bbool_c;

extern bbool_c* (*bbool_c_impl)(void);

/**
 * \extends bobj_t
 * Wrapper class over a `bool`
 */
typedef struct {
    bobj_t super;
    bool val;
} bbool_t;

void bbool_new(bbool_t *self, bool val);
bbool_t* h_bbool(bool val);
bbool_t s_bbool(bool val);
