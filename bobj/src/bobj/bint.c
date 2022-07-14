#include "bint.h"
#include "bobj.h"

#include <malloc.h>

static void bint_drop(bobj_t *obj) {}

static inline void typecheck(bobj_t *lhs, bobj_t *rhs, char * op) {
    if(
        !bobj_instanceof(bint_v_impl(), lhs) || 
        !bobj_instanceof(bint_v_impl(), rhs)
    ) {
        bobj_panic("Cannot %s %s to %s", op, vft_cast(bobj_v, lhs)->name, vft_cast(bobj_v, rhs)->name); 
    }
}

#define BINT_OP(name, op) \
static void bint_##name (bobj_t *lhs, bobj_t *rhs, bobj_t *res) {       \
    typecheck(lhs, rhs, #name);                                         \
    bint_new((bint_t*)res, ((bint_t*)lhs)->val op ((bint_t*)rhs)->val); \
}

BINT_OP(add, +)
BINT_OP(sub, -)
BINT_OP(mul, *)
BINT_OP(div, /)

#undef BINT_OP


vft_creator(
    bint_v,
    bint_v_impl,
    (bint_v){
        .super = (bobj_v){
            .drop = bint_drop,
            .size = sizeof(int),
            .name = "bint",
        },
        .add = (add_i){.add = bint_add},
        .sub = (sub_i){.sub = bint_sub},
        .mul = (mul_i){.mul = bint_mul},
        .div = (div_i){.div = bint_div},
    }
)

void bint_new(bint_t *self, int val) {
    vft_cast(bint_v, self) = bint_v_impl();
    self->val = val;
}
