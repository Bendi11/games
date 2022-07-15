#include "iter.h"

#include <malloc.h>

vft_creator(
    biter_c,
    biter_c_impl,
    (biter_c){
        .super = (bobj_c){
            .name = "biter",
            .size = 0,
            .drop = bobj_virtual,
        },
        .next = (void*)bobj_virtual
    }
)

bobj_t* blist_iter_next(blist_iter_t *self) {
    if(self->i >= blist_len(self->list)) {
        return NULL;
    } else {
        return blist_at(self->list, self->i++);
    }
}
