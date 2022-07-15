#include "list.h"
#include "bobj.h"

#include <malloc.h>

vft_creator(
    blist_c,
    blist_c_impl,
    (blist_c){
        .super = (bobj_c){
            .name = "blist",
            .drop = bobj_virtual,
            .size = 0,
        },
        .at = (void*)bobj_virtual,
        .len = (void*)bobj_virtual,
    }
)

static bobj_t* bsingle_list_at(blist_t *self, size_t idx) {
    if(idx == 0) {
        return ((bsingle_list_t*)(self))->elem;
    } else {
        return NULL;
    }
}

static size_t bsingle_list_len(blist_t *self) {
    return 1;
}

static void bsingle_list_drop(bobj_t *oself) {
    bsingle_list_t *self = (bsingle_list_t*)oself;
    bobj_drop(self->elem);
}

vft_creator(
    bsingle_list_c,
    bsingle_list_c_impl,
    (bsingle_list_c){
        .super = (blist_c){
            .at = bsingle_list_at,
            .len = bsingle_list_len,
            .super = (bobj_c){
                .size = sizeof(bsingle_list_t) - sizeof(bsingle_list_c*),
                .drop = bsingle_list_drop,
                .name = "bsingle_list"
            }
        }
    }
)
