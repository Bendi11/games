#pragma once

#include "bobj.h"
#include "bobj/ops.h"
#include <stdint.h>

#define BOBJ_DEC_PRIMITIVE(ty,contained)                         \
typedef struct b##ty##_c {                                       \
    bobj_c super;                                                \
    add_i add;                                                   \
    sub_i sub;                                                   \
    mul_i mul;                                                   \
    div_i div;                                                   \
} b##ty##_c;                                                     \
                                                                 \
/** \brief Get the default vtable for the bint_t type */         \
extern b##ty##_c* (*b##ty##_c_impl)(void);                       \
                                                                 \
typedef struct b##ty##_t {                                       \
    bobj_t super;                                                \
    contained val;                                               \
} b##ty##_t;                                                     \
                                                                 \
/** \brief Initialize an instance of class `bint` */             \
void b##ty##_new(b##ty##_t *, contained val);                    \
b##ty##_t s_b##ty(contained val);                                \
b##ty##_t* h_b##ty(contained val);                               \


BOBJ_DEC_PRIMITIVE(char, char)
BOBJ_DEC_PRIMITIVE(short, short)
BOBJ_DEC_PRIMITIVE(int, int)
BOBJ_DEC_PRIMITIVE(long, long)

BOBJ_DEC_PRIMITIVE(u8, uint8_t)
BOBJ_DEC_PRIMITIVE(u16, uint16_t)
BOBJ_DEC_PRIMITIVE(u32, uint32_t)
BOBJ_DEC_PRIMITIVE(u64, uint64_t)
BOBJ_DEC_PRIMITIVE(i8, int8_t)
BOBJ_DEC_PRIMITIVE(i16, int16_t)
BOBJ_DEC_PRIMITIVE(i32, int32_t)
BOBJ_DEC_PRIMITIVE(i64, int64_t)

BOBJ_DEC_PRIMITIVE(size, size_t)


#undef BOBJ_DEC_PRIMITIVE
