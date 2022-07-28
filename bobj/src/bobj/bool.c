#include "bobj/bool.h"
#include "bobj.h"
#include "bobj/list.h"
#include "bobj/ops.h"
#include <malloc.h>

void bbool_eq_impl(blist_t *args, bobj_t *res) {
    bbool_t *lhs = (bbool_t*)blist_at(args, 0);
    bbool_t *rhs = (bbool_t*)blist_at(args, 1);
    ((bbool_t*)res)->val = lhs->val == rhs->val;
}

vft_creator(
    bbool_c,
    bbool_c_impl,
    *self_class = (bbool_c){
        .super = (bobj_c){
            .drop = bobj_default_drop,
            .name = "bbool",
            .size = sizeof(bbool_t) - sizeof(bbool_c*),
            .traits = btraitmap_create((btrait_t*[]){
                (btrait_t*)h_bieq(
                    (bfn_t*)h_bfnptr_fn(
                        bbool_eq_impl,
                        (blist_t*)h_bbuflist_create((bobj_t*)h_bobj_class((bobj_c*)bbool_c_impl()), (bobj_t*)h_bobj_class((bobj_c*)bbool_c_impl())),
                        (bobj_c*)bbool_c_impl()
                    )
                )
            })
        }
    };
)

void bbool_new(bbool_t *self, bool val) {
    vft_cast(bbool_c, self) = bbool_c_impl();
    self->val = val;
}

bbool_t *h_bbool(bool val) {
    bbool_t *self = malloc(sizeof(*self));
    bbool_new(self, val);
    return self;
}

bbool_t s_bbool(bool val) {
    bbool_t self;
    bbool_new(&self, val);
    return self;
}
