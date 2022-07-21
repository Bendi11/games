#include "bobj/ops.h"

#include <malloc.h>

vft_creator(
    btrait_c,
    biadd_c_impl,
    (btrait_c){
        .super = (bobj_c){
            .name = "add_i",
            .traits = s_btraitlist(),
            .size = sizeof(biadd_t) - sizeof(btrait_c*),
            .drop = bobj_default_drop,
        },
        .id = btrait_newid(),
    }
)

void biadd_new(biadd_t *self, add_i_addfn fn) {
    vft_cast(btrait_c, self) = biadd_c_impl();
    self->add = fn;
}
biadd_t* h_biadd(add_i_addfn fn) {
    biadd_t *add = malloc(sizeof(*add));
    biadd_new(add, fn);
    return add;
}
