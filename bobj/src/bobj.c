#include "bobj.h"

#include <malloc.h>
#include <stdlib.h>


static void bint_drop(bobj_t *obj) {}

static bobj_t* bint_add(bobj_t *self_o, bobj_t *other_o) {
    bint_t *self = (bint_t*)self_o;
    bint_t *other = (bint_t*)other_o;

    bint_t *res = malloc(sizeof(*res));
    bint_new(res, self->val + other->val);
    return (bobj_t*)res;
}

void bobj_drop(bobj_t *obj) {
    vft_cast(bobj_v, obj)->drop(obj);
}

vft_creator(
    bint_v,
    bint_v_impl,
    (bint_v){
        .super = (bobj_v){
            .drop = bint_drop,
            .size = sizeof(int),
        },
        .add_impl = (add_i){
            .add = bint_add,
        }
    }
)

void bint_new(bint_t *self, int val) {
    vft_cast(bint_v, self) = bint_v_impl();
    self->val = val;
}

BOBJ_NORETURN void bobj_virtual() {
    fprintf(stderr, "Invoked virtual method"); 
    exit(-1);
}
