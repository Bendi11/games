#pragma once

#include "bobj.h"
#include <stdint.h>


#define h_bbuflist_create(...) (h_bbuf_list((sizeof((bobj_t*[]){__VA_ARGS__}) / sizeof(((bobj_t*[]){__VA_ARGS__}))), (bobj_t*[]{__VA_ARGS__})))
#define s_bbuflist_create(...) (s_bbuf_list((sizeof((bobj_t*[]){__VA_ARGS__}) / sizeof(((bobj_t*[]){__VA_ARGS__}))), (bobj_t*[]{__VA_ARGS__})))

typedef struct blist_t blist_t;
typedef struct blist_c {
    bobj_c super;
    bobj_t* (*at)(blist_t *,size_t);
    size_t (*len)(blist_t*);
} blist_c;

typedef struct blist_t {
    bobj_t super;
} blist_t;

bobj_t* blist_at(blist_t* list, size_t i);
size_t blist_len(blist_t* list);
extern blist_c* (*blist_c_impl)(void);

typedef struct bmutlist_t bmutlist_t;
typedef struct bmutlist_c {
    blist_c super;
    void (*add)(bmutlist_t *, bobj_t*);
} bmutlist_c;

extern bmutlist_c* (*bmutlist_c_impl)(void);

/**
 * \brief Class that all mutable lists derive from
 */
typedef struct bmutlist_t {
    blist_t super;
} bmutlist_t;

void bmutlist_add(bmutlist_t *list, bobj_t *elem);

typedef struct bsingle_list_t bsingle_list_t;
typedef struct bsingle_list_c {
    blist_c super;
} bsingle_list_c;

typedef struct bsingle_list_t {
    blist_t super;
    bobj_t *elem;
} bsingle_list_t;

extern bsingle_list_c* (*bsingle_list_c_impl)(void);
void bsingle_list_new(bsingle_list_t *list, bobj_t *elem);
bsingle_list_t s_bsingle_list(bobj_t *elem);
bsingle_list_t* h_bsingle_list(bobj_t *elem);

typedef struct bbuf_list_t bbuf_list_t;
typedef struct {
    bmutlist_c super;
} bbuf_list_c;

extern bbuf_list_c* (*bbuf_list_c_impl)(void);

typedef struct bbuf_list_t {
    bmutlist_t super;
    bobj_t **_buf;
    uint32_t len;
    uint32_t _cap;
} bbuf_list_t;

void bbuf_list_new(bbuf_list_t *self, size_t len, bobj_t* elems[]);
bbuf_list_t s_bbuf_list(size_t len, bobj_t* elems[]);
bbuf_list_t* h_bbuf_list(size_t len, bobj_t* elems[]);
