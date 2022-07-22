#include "bobj/ops.h"
#include "bobj.h"

#include <malloc.h>

vft_creator(
    btrait_c,
    biadd_c_impl,
    (btrait_c){
        .super = (bobj_c){
            .name = "biadd",
            .traits = s_btraitmap(),
            .size = sizeof(biadd_t) - sizeof(btrait_c*),
            .drop = bobj_default_drop,
        },
        .id = btrait_newid(),
    }
)

vft_creator(
    btrait_c,
    bisub_c_impl,
    (btrait_c){
        .super = (bobj_c){
            .name = "bisub",
            .traits = s_btraitmap(),
            .size = sizeof(bisub_t) - sizeof(btrait_c*),
            .drop = bobj_default_drop,
        },
        .id = btrait_newid(),
    }
)

vft_creator(
    btrait_c,
    bimul_c_impl,
    (btrait_c){
        .super = (bobj_c){
            .name = "bimul",
            .traits = s_btraitmap(),
            .size = sizeof(bimul_t) - sizeof(btrait_c*),
            .drop = bobj_default_drop,
        },
        .id = btrait_newid(),
    }
)

vft_creator(
    btrait_c,
    bidiv_c_impl,
    (btrait_c){
        .super = (bobj_c){
            .name = "bidiv",
            .traits = s_btraitmap(),
            .size = sizeof(bidiv_t) - sizeof(btrait_c*),
            .drop = bobj_default_drop,
        },
        .id = btrait_newid(),
    }
)

void biadd_new(biadd_t *self, bfn_t *fn) {
    vft_cast(btrait_c, self) = biadd_c_impl();
    self->add = fn;
}
biadd_t* h_biadd(bfn_t *fn) {
    biadd_t *add = malloc(sizeof(*add));
    biadd_new(add, fn);
    return add;
}

void bisub_new(bisub_t *self, bfn_t *fn) {
    vft_cast(btrait_c, self) = bisub_c_impl();
    self->sub = fn;
}
bisub_t* h_bisub(bfn_t *fn) {
    bisub_t *sub = malloc(sizeof(*sub));
    bisub_new(sub, fn);
    return sub;
}

void bimul_new(bimul_t *self, bfn_t *fn) {
    vft_cast(btrait_c, self) = bimul_c_impl();
    self->mul = fn;
}
bimul_t* h_bimul(bfn_t *fn) {
    bimul_t *mul = malloc(sizeof(*mul));
    bimul_new(mul, fn);
    return mul;
}

void bidiv_new(bidiv_t *self, bfn_t *fn) {
    vft_cast(btrait_c, self) = bidiv_c_impl();
    self->div = fn;
}
bidiv_t* h_bidiv(bfn_t *fn) {
    bidiv_t *div = malloc(sizeof(*div));
    bidiv_new(div, fn);
    return div;
}
