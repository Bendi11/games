#pragma once

#include "bobj.h"

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
