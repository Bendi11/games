#pragma once

#include "bobj.h"
#include "bobj/io/write.h"

typedef void (*bfmt_i_fmt_fn)(bobj_t*, bwriter_t*);

/** 
 * \brief Trait implemented by all types that can be written a
 * `bwriter_t`
 */
typedef struct {
    btrait_t super;
    bfmt_i_fmt_fn fmt;
} bifmt_t;

extern btrait_c* (*bifmt_c_impl)(void);
void bifmt_new(bifmt_t *self, bfmt_i_fmt_fn fmt);
bifmt_t* h_bifmt(bfmt_i_fmt_fn fmt);
