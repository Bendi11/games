#include "ops.h"

#include <malloc.h>

vft_creator(
    btrait_c,
    add_i_c_impl,
    (btrait_c){
        .super = (bobj_c){
            .name = "add_i",
            .traits = s_btraitlist(),
            .size = sizeof(add_i_t) - sizeof(btrait_c*),
            .drop = bobj_default_drop,
        },
        .id = btrait_newid(),
    }
)

void add_i_new(add_i_t *self, add_i_addfn fn) {
    vft_cast(btrait_c, self) = add_i_c_impl();
    self->add = fn;
}
add_i_t* h_add_i(add_i_addfn fn) {
    add_i_t *add = malloc(sizeof(*add));
    add_i_new(add, fn);
    return add;
}
