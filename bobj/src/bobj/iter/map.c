#include "bobj/iter/map.h"
#include "bobj/list.h"
#include <malloc.h>

static bobj_t* biter_map_next(biter_t *iter) {
    biter_map_t *self = (biter_map_t*)iter;
    bobj_t *next = biter_next(self->iter);
    if(next != NULL) {
        bsingle_list_t elem;
        bsingle_list_new(&elem, next);
        bobj_t *res = malloc(bfn_return_ty(self->map)->size + sizeof(bobj_c*));
        bfn_call(self->map, (blist_t*)&elem, res);
        return res;
    } else {
        return NULL;
    }
}

static void biter_map_drop(bobj_t *o) {
    biter_map_t *self = (biter_map_t*)o;
    bobj_drop((bobj_t*)self->map);
    bobj_drop((bobj_t*)self->iter);
}

vft_creator(
    biter_map_c,
    biter_map_c_impl,
    *self_class = (biter_map_c){
        .super = (biter_c){
            .next = biter_map_next,
            .super = (bobj_c){
                .name = "biter_map",
                .drop = biter_map_drop,
                .size = sizeof(biter_map_t) - sizeof(biter_map_c*)
            }
        }
    };
)

void biter_map_new(biter_map_t *self, bfn_t *map, biter_t *iter) {
    vft_cast(biter_map_c, self) = biter_map_c_impl();
    self->map = map;
    self->iter = iter;
}

biter_map_t s_biter_map(biter_t *iter, bfn_t *mapper) {
    biter_map_t map;
    biter_map_new(&map, mapper, iter);
    return map;
}
biter_map_t* h_biter_map(biter_t *iter, bfn_t *mapper) {
    biter_map_t *map = malloc(sizeof(*map));
    biter_map_new(map, mapper, iter);
    return map;
}
