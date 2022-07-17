#include "int.h"	
#include "bobj.h"	
#include <malloc.h>	

static void empty_drop(bobj_t* obj) {}

#define BOBJ_DEF_PRIMITIVE(ty, contained)	\
static inline void b##ty##_typecheck(bobj_t *lhs, bobj_t *rhs, char * op) {	\
    if(	\
        !bobj_instanceof(b##ty##_c_impl(), lhs) || 	\
        !bobj_instanceof(b##ty##_c_impl(), rhs)	\
    ) {	\
        bobj_panic("Cannot %s %s to %s", op, vft_cast(bobj_c, lhs)->name, vft_cast(bobj_c, rhs)->name); 	\
    }	\
}	\
	\
static void b##ty##_add(bobj_t *lhs, bobj_t *rhs, bobj_t *res) {       \
    b##ty##_typecheck(lhs, rhs, "add");                                         \
    b##ty##_new((b##ty##_t*)res, ((b##ty##_t*)lhs)->val + ((b##ty##_t*)rhs)->val); \
}\
static void b##ty##_sub(bobj_t *lhs, bobj_t *rhs, bobj_t *res) {       \
    b##ty##_typecheck(lhs, rhs, "subtract");                                         \
    b##ty##_new((b##ty##_t*)res, ((b##ty##_t*)lhs)->val - ((b##ty##_t*)rhs)->val); \
}\
static void b##ty##_mul(bobj_t *lhs, bobj_t *rhs, bobj_t *res) {       \
    b##ty##_typecheck(lhs, rhs, "multiply");                                         \
    b##ty##_new((b##ty##_t*)res, ((b##ty##_t*)lhs)->val * ((b##ty##_t*)rhs)->val); \
}\
static void b##ty##_div(bobj_t *lhs, bobj_t *rhs, bobj_t *res) {       \
    b##ty##_typecheck(lhs, rhs, "divide");                                         \
    b##ty##_new((b##ty##_t*)res, ((b##ty##_t*)lhs)->val / ((b##ty##_t*)rhs)->val); \
}   \
    \
	\
vft_creator(	\
    b##ty##_c,	\
    b##ty##_c_impl,	\
    (b##ty##_c){	\
        .super = (bobj_c){	\
            .drop = empty_drop,	\
            .size = sizeof(contained),	\
            .name = "b" #ty,	\
            .traits = btraitlist_add(s_btraitlist(), (btrait_t*)h_biadd(b##ty##_add)), \
        },	\
    }	\
)	\
	\
void b##ty##_new(b##ty##_t *self, contained val) {	\
    vft_cast(b##ty##_c, self) = b##ty##_c_impl();	\
    self->val = val;	\
}	\
b##ty##_t* h_b##ty(contained val) {	\
    b##ty##_t* i = malloc(sizeof(*i));	\
    b##ty##_new(i, val);	\
    return i;	\
}	\
b##ty##_t s_b##ty(contained val) {	\
    b##ty##_t i;	\
    b##ty##_new(&i, val);	\
    return i;	\
}	

BOBJ_DEF_PRIMITIVE(char, char)
BOBJ_DEF_PRIMITIVE(short, short)
BOBJ_DEF_PRIMITIVE(int, int)
BOBJ_DEF_PRIMITIVE(long, long)

BOBJ_DEF_PRIMITIVE(u8, uint8_t)
BOBJ_DEF_PRIMITIVE(u16, uint16_t)
BOBJ_DEF_PRIMITIVE(u32, uint32_t)
BOBJ_DEF_PRIMITIVE(u64, uint64_t)
BOBJ_DEF_PRIMITIVE(i8, int8_t)
BOBJ_DEF_PRIMITIVE(i16, int16_t)
BOBJ_DEF_PRIMITIVE(i32, int32_t)
BOBJ_DEF_PRIMITIVE(i64, int64_t)

BOBJ_DEF_PRIMITIVE(size, size_t)
