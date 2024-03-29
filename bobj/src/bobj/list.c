#include "bobj/list.h"
#include "bobj.h"
#include "bobj/iter.h"

#include <malloc.h>
#include <memory.h>

vft_creator(
    blist_c,
    blist_c_impl,
    *self_class = (blist_c){
        .super = (bobj_c){
            .name = "blist",
            .drop = bobj_virtual,
            .size = 0,
        },
        .at = (void*)bobj_virtual,
        .len = (void*)bobj_virtual,
    };
)

vft_creator(
    bmutlist_c,
    bmutlist_c_impl,
    *self_class = (bmutlist_c){
        .add = (void*)bobj_virtual,
        .super = *blist_c_impl(),
    };
)

bobj_t* blist_at(blist_t *list, size_t idx) { return vft_cast(blist_c, list)->at(list, idx); }
size_t blist_len(blist_t *list) { return vft_cast(blist_c, list)->len(list); }

void bmutlist_add(bmutlist_t *list, bobj_t *elem) { vft_cast(bmutlist_c, list)->add(list, elem); }

void bsingle_list_new(bsingle_list_t *list, bobj_t *elem) {
    vft_cast(bsingle_list_c, list) = bsingle_list_c_impl();
    list->elem = elem;
}

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

bsingle_list_t* h_bsingle_list(bobj_t* elem) {
    bsingle_list_t *list = malloc(sizeof(*list));
    bsingle_list_new(list, elem);
    return list;
}
bsingle_list_t s_bsingle_list(bobj_t* elem) {
    bsingle_list_t list;
    bsingle_list_new(&list, elem);
    return list;
}



vft_creator(
    bsingle_list_c,
    bsingle_list_c_impl,
    *self_class = (bsingle_list_c){
        .super = (blist_c){
            .at = bsingle_list_at,
            .len = bsingle_list_len,
            .super = (bobj_c){
                .size = sizeof(bsingle_list_t) - sizeof(bsingle_list_c*),
                .drop = bsingle_list_drop,
                .name = "bsingle_list"
            }
        }
    };
)

static bobj_t* bdouble_list_at(blist_t *self, size_t idx) {

}

void bdouble_list_new(bdouble_list_t *list, bobj_t *elem, bobj_t *elems[2]);
bdouble_list_t s_bdouble_list(bobj_t *elems[2]);
bdouble_list_t* h_bdouble_list(bobj_t *elems[2]);

vft_creator(
    bdouble_list_c,
    bdouble_list_c_impl,
    *self_class = (bdouble_list_c){
        .super = (blist_c){
            .at = bdouble_list_at,
        }
    };
)

static void bbuf_list_add(bmutlist_t *list, bobj_t *elem) {
    bbuf_list_t *self = (bbuf_list_t*)list;
    if(self->len >= self->_cap) {
        self->_cap = self->len * 2;
        self->_buf = realloc(self->_buf, self->_cap * sizeof(bobj_t*));
    }
    self->_buf[self->len] = elem;
    self->len += 1;
}

static size_t bbuf_list_len(blist_t *list) {
    bbuf_list_t *self = (bbuf_list_t*)list;
    return self->len;
}

static bobj_t* bbuf_list_at(blist_t *list, size_t idx) {
    bbuf_list_t *self = (bbuf_list_t*)list;
    return self->_buf[idx];
}

static void bbuf_list_drop(bobj_t *obj) {
    bbuf_list_t *self = (bbuf_list_t*)obj;
    blist_iter_t iter = s_blist_iter((blist_t*)self);
    bobj_t *next = NULL;
    while((next = biter_next((biter_t*)&iter)) != NULL) {
        bobj_drop(next); 
    }
    free(self->_buf);
}

vft_creator(
    bbuf_list_c,
    bbuf_list_c_impl,
    *self_class = (bbuf_list_c){
        .super = (bmutlist_c){
            .super = (blist_c){
                .super = (bobj_c){
                    .name = "bbuf_list",
                    .drop = bbuf_list_drop,
                    .size = sizeof(bbuf_list_t) - sizeof(bbuf_list_c*),
                },
                .len = bbuf_list_len,
                .at = bbuf_list_at,
            },
            .add = bbuf_list_add,
        },
    };
)

void bbuf_list_new(bbuf_list_t *self, size_t len, bobj_t **elems) {
    vft_cast(bbuf_list_c, self) = bbuf_list_c_impl();
    self->len = len;
    self->_cap = len ? len : 1;
    self->_buf = calloc(self->_cap, sizeof(bobj_t*));
    len > 0 && memcpy(self->_buf, elems, len * sizeof(bobj_t*));
}

bbuf_list_t* h_bbuf_list(size_t len, bobj_t **elems) {
    bbuf_list_t *list = malloc(sizeof(*list));
    bbuf_list_new(list, len, elems);
    return list;
}

bbuf_list_t s_bbuf_list(size_t len, bobj_t **elems) {
    bbuf_list_t list;
    bbuf_list_new(&list, len, elems);
    return list;
}
