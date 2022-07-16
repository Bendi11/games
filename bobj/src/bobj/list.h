#pragma once

#include "bobj.h"
#include <stdint.h>

typedef struct blist_t blist_t;
typedef struct blist_c {
    bobj_c super;
    bobj_t* (*at)(blist_t *,size_t);
    size_t (*len)(blist_t*);
} blist_c;

typedef struct blist_t {
    bobj_t super;
} blist_t;


inline bobj_t* blist_at(blist_t* list, size_t i) { return vft_cast(blist_c, list)->at(list, i); }
inline size_t blist_len(blist_t* list) { return vft_cast(blist_c, list)->len(list); }
extern blist_c* (*blist_c_impl)(void);

typedef struct bsingle_list_t bsingle_list_t;
typedef struct bsingle_list_c {
    blist_c super;
} bsingle_list_c;

typedef struct bsingle_list_t {
    blist_t super;
    bobj_t *elem;
} bsingle_list_t;

extern bsingle_list_c* (*bsingle_list_c_impl)(void);
inline void bsingle_list_new(bsingle_list_t *list, bobj_t *elem) {
    vft_cast(bsingle_list_c, list) = bsingle_list_c_impl();
    list->elem = elem;
}
inline bsingle_list_t s_bsingle_list(bobj_t *elem) {
    bsingle_list_t list;
    bsingle_list_new(&list, elem);
    return list;
}
bsingle_list_t* h_bsingle_list(bobj_t *elem);

typedef struct bbuf_list_t bbuf_list_t;
typedef struct {
    blist_c super;
} bbuf_list_c;

extern bbuf_list_c* (*bbuf_list_c_impl)(void);

typedef struct bbuf_list_t {
    blist_t super;
    bobj_t **_buf;
    uint32_t len;
    uint32_t _cap;
} bbuf_list_t;

void bbuf_list_new(bbuf_list_t *self, size_t len, bobj_t* elems[]);
inline bbuf_list_t s_bbuf_list(size_t len, bobj_t* elems[]) {
    bbuf_list_t list;
    bbuf_list_new(&list, len, elems);
    return list;
}
bbuf_list_t* h_bbuf_list(size_t len, bobj_t* elems[]);
