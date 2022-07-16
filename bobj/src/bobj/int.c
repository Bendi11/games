#include "int.h"
#include "bobj.h"

#include <malloc.h>

static void bint_drop(bobj_t *obj) {}

static inline void typecheck(bobj_t *lhs, bobj_t *rhs, char * op) {
    if(
        !bobj_instanceof(bint_c_impl(), lhs) || 
        !bobj_instanceof(bint_c_impl(), rhs)
    ) {
        bobj_panic("Cannot %s %s to %s", op, vft_cast(bobj_c, lhs)->name, vft_cast(bobj_c, rhs)->name); 
    }
}

#define BINT_OP(name, op)                                               \
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
    bint_c,
    bint_c_impl,
    (bint_c){
        .super = (bobj_c){
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
    vft_cast(bint_c, self) = bint_c_impl();
    self->val = val;
}

bint_t* h_bint(int val) {
    bint_t* i = malloc(sizeof(*i));
    bint_new(i, val);
    return i;
}

bint_t s_bint(int val) {
    bint_t i;
    bint_new(&i, val);
    return i;
}
