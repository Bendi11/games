#include "bobj.h"

#include <malloc.h>
#include <stdarg.h>
#include <stdlib.h>

vft_creator(
    bobj_c,
    bobj_c_impl,
    (bobj_c){
        .size = 0,
        .drop = bobj_default_drop,
        .traits = s_btraitlist(),
        .name = "bobj"
    }
)

void bobj_drop(bobj_t *obj) {
    vft_cast(bobj_c, obj)->drop(obj);
}

size_t bobj_size(bobj_t *obj) { return vft_cast(bobj_c, obj)->size; }
const char* bobj_name(bobj_t *obj) { return vft_cast(bobj_c, obj)->name; }


vft_creator(
    btrait_c,
    btrait_c_impl,
    (btrait_c){
        .super = (bobj_c){
            .name = "btrait",
            .size = sizeof(btrait_t) - sizeof(btrait_c*),
            .drop = bobj_default_drop,
            .traits = s_btraitlist(),
        },
    }
)

static void btraitlist_new_n(btraitlist_t *list, btrait_t **traits, size_t len) {
    list->_traits = calloc(len, sizeof(btrait_t*));
    list->_len = len;
}


btraitlist_t btraitlist_add(btraitlist_t list, btrait_t *trait) {
    for(size_t i = 0; i < list._len; ++i) {
        if(vft_cast(btrait_c, list._traits[i])->id == vft_cast(btrait_c, trait)->id) {
            list._traits[i] = trait;
        }
    }
    
    list._len += 1;
    list._traits = realloc(list._traits, list._len * sizeof(btrait_t*));
    list._traits[list._len - 1] = trait;
    return list;
}

static btraitlist_t btraitlist_combine(btraitlist_t existing, btraitlist_t *other) {
    for(size_t i = 0; i < other->_len; ++i) {
        existing = btraitlist_add(existing, other->_traits[i]);
    }
    return existing;
}

btraitlist_t btraitlist_extendn(btraitlist_t existing, btrait_t **traits, size_t len) {
    btraitlist_t new;
    btraitlist_new_n(&new, traits, len);
    new = btraitlist_combine(new, &existing); 
    return new;
}

btrait_t* btrait_get(bobj_c *classty, btrait_id_t hash) {
    for(size_t i = 0; i < classty->traits._len; ++i) {
        if(vft_cast(btrait_c, classty->traits._traits[i])->id == hash) {
            return classty->traits._traits[i];
        }
    }
    return 0;
}

btraitlist_t s_btraitlist(void) {
    btraitlist_t list;
    btraitlist_new_n(&list, NULL, 0);
    return list;
}

btrait_id_t btrait_newid(void) {
    static btrait_id_t idx = 0;
    return idx++;
}

BOBJ_NORETURN void bobj_virtual() {
    bobj_panic("Attempt to invoke a virtual method");
}

void bobj_default_drop(bobj_t *obj) {}

void bobj_panic(const char *message, ...) {
    va_list list;
    va_start(list, message);
    
    vfprintf(stderr, message, list);
    fputs("", stderr);
    va_end(list);
    exit(-1);
}
