#pragma once

#include "bobj.h"


typedef struct bwriter_t bwriter_t;
typedef struct  {
    bobj_c super;
    size_t (*write)(bwriter_t*,uint8_t*,size_t);
} bwriter_c;

extern bwriter_c* (*bwriter_c_impl)(void);

/**
 * \brief Abstract base class for all structures that support writing bytes
 */
typedef struct bwriter_t {
    bobj_t super;
} bwriter_t;
