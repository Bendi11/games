#pragma once

#include "bobj.h"

typedef struct bstr_t bstr_t;
typedef struct bstr_c {
    bobj_c super;
    size_t (*len)(bstr_t*);
    char* (*cstr)(bstr_t*);
    void (*append)(bstr_t*,bstr_t*);
} bstr_c;

typedef struct bstr_t {
    bobj_t super;
} bstr_t;

extern bstr_c* (*bstr_c_impl)(void);
