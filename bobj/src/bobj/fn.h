#pragma once

#include "bobj.h"

#include <stdbool.h>


typedef struct fn_t fn_t;
typedef struct fn_v {
    bobj_v super;
    bool (*typecheck)(fn_t *self, size_t arg_count, bobj_t *args[]);
    bobj_t* (*call)(fn_t *self, size_t arg_count, bobj_t *args[]);
} fn_v;

typedef struct fn_t {
    bobj_v super;
} fn_t;

