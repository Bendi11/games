#include "iter.h"
#include "bobj.h"
#include "bobj/list.h"

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

bobj_t* biter_fold(biter_t *self, bobj_t *acc, bfn_t *fold) {
    bobj_t *next;
    while((next = biter_next(self)) != NULL) {
        bobj_t *argarray[2] = {acc, next};
        bbuf_list_t args = s_bbuf_list(2, argarray);
        bfn_call(fold, (blist_t*)&args, NULL); 
    }
    return acc;
}

static bobj_t* blist_iter_next(biter_t *iter) {
    blist_iter_t *self = (blist_iter_t*)iter;
    if(self->i >= blist_len(self->list)) {
        return NULL;
    } else {
        return blist_at(self->list, self->i++);
    }
}

static void blist_iter_drop(bobj_t *o) {
    //blist_iter_t *self = (blist_iter_t*)o;
}

vft_creator(
    blist_iter_c,
    blist_iter_c_impl,
    (blist_iter_c){
        .super = (biter_c){
            .next = blist_iter_next,
            .super = (bobj_c){
                .size = sizeof(blist_iter_t) - sizeof(blist_iter_c*),
                .name = "blist_iter",
                .drop = blist_iter_drop
            }
        } 
    }
)

void blist_iter_new(blist_iter_t *self, blist_t *list) {
    vft_cast(blist_iter_c, self) = blist_iter_c_impl();
}
blist_iter_t s_blist_iter(blist_t *list) {
    blist_iter_t iter;
    blist_iter_new(&iter, list);
    return iter;
}
blist_iter_t* h_blist_iter(blist_t *list) {
    blist_iter_t *iter = malloc(sizeof(*iter));
    blist_iter_new(iter, list);
    return iter;
}
