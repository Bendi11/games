#include "bobj/fmt.h"
#include "bobj.h"

#include <malloc.h>

vft_creator(
    btrait_c,
    bfmt_c_impl,
    (btrait_c){
        .super = (bobj_c){
            .size = sizeof(bifmt_t) - sizeof(btrait_c*),
            .drop = bobj_default_drop,
            .traits = s_btraitlist(),
            .name = "bfmt"
        },
        .id = btrait_newid(),
    }
)

void bifmt_new(bifmt_t *self, bfmt_i_fmt_fn fmt) {
    vft_cast(btrait_c, self) = bfmt_c_impl();
    self->fmt = fmt;
}
bifmt_t *h_bifmt(bfmt_i_fmt_fn fmt) {
    bifmt_t *trait = malloc(sizeof(*fmt));
    bifmt_new(trait, fmt);
    return trait;
}
