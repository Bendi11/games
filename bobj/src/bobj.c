#include "bobj.h"

#include <malloc.h>
#include <stdarg.h>
#include <stdlib.h>
#include <memory.h>

vft_creator(
    bobj_c,
    bobj_c_impl,
    *self_class = (bobj_c){
        .size = 0,
        .drop = bobj_default_drop,
        .traits = s_btraitmap(),
        .name = "bobj"
    };
)

void bobj_drop(bobj_t *obj) {
    vft_cast(bobj_c, obj)->drop(obj);
}

size_t bobj_size(bobj_t *obj) { return vft_cast(bobj_c, obj)->size; }
const char* bobj_name(bobj_t *obj) { return vft_cast(bobj_c, obj)->name; }

vft_creator(
    bclass_c,
    bclass_c_impl,
    *self_class = (bclass_c){
        .super = (bobj_c){
            .drop = bobj_default_drop,
            .size = sizeof(bclass_t) - sizeof(bclass_c*),
            .name = "bclass",
            .traits = s_btraitmap(),
        }
    };
)

void bclass_new(bclass_t *self, bobj_c *classty) {
    vft_cast(bclass_c, self) = bclass_c_impl();
}
bclass_t s_bobj_class(bobj_c *classty) {
    bclass_t self;
    bclass_new(&self, classty);
    return self;
}
bclass_t* h_bobj_class(bobj_c *classty) {
    bclass_t *self = malloc(sizeof(*self));
    bclass_new(self, classty);
    return self;
}

vft_creator(
    btrait_c,
    bifmt_c_impl,
    *self_class = (btrait_c){
        .super = (bobj_c){
            .name = "btrait",
            .size = sizeof(btrait_t) - sizeof(btrait_c*),
            .drop = bobj_default_drop,
            .traits = s_btraitmap(),
        },
    };
)


typedef uint32_t btraitmap_idx_t;
#define INVALID_TRAITIDX ((btraitmap_idx_t)-1)

/** Get an index in the btraitmap_t's internal array for the given id */
btraitmap_idx_t btraitmap_slot(btraitmap_t *map, btrait_id_t id) {
    btraitmap_idx_t idx = id % map->_len;
    while(map->_traits[idx] != NULL && vft_cast(btrait_c, map->_traits[idx])->id != id) {
        idx = (idx + 1) % map->_len;
    }
    return idx;
}

void btraitmap_add(btraitmap_t *map, btrait_t *trait) {
    if(map->_len / 2 <= map->_occupied) {
        btraitmap_t newmap;
        newmap._len = map->_len * 2;
        newmap._occupied = map->_occupied;
        newmap._traits = calloc(newmap._len, sizeof(btrait_t*));
        for(btraitmap_idx_t i = 0; i < map->_len; ++i) {
            if(map->_traits[i] == NULL) continue;
            btraitmap_add(&newmap, map->_traits[i]);
        }
        memcpy(map, &newmap, sizeof(newmap));
    }
    map->_occupied += 1;
    map->_traits[btraitmap_slot(map, vft_cast(btrait_c, trait)->id)] = trait; 
}

void btraitmap_new_n(btraitmap_t *map, btrait_t **traits, size_t len) {
    map->_traits = calloc(len * 2, sizeof(btrait_t*));
    map->_len = len * 2;
    map->_occupied = len;
    for(size_t i = 0; i < len; ++i) {
        btraitmap_add(map, traits[i]);
    }
}


void btraitmap_extendn(btraitmap_t* existing, btrait_t **traits, size_t len) {
    for(btraitmap_idx_t i = 0; i < existing->_len; ++i) {
        btraitmap_add(existing, existing->_traits[i]);
    }
}

btrait_t* btrait_get(bobj_c *classty, btrait_id_t hash) {
    btraitmap_idx_t idx = btraitmap_slot(&classty->traits, hash);
    return classty->traits._traits[idx];
}

btraitmap_t btraitmap_combine(btraitmap_t existing, btraitmap_t other) {
    for(btraitmap_idx_t i = 0; i < other._len; ++i) {
        if(other._traits[i] == NULL) continue;
        btraitmap_idx_t idx = btraitmap_slot(&existing, vft_cast(btrait_c, other._traits[i])->id);
        if(existing._traits[idx] != NULL) continue;
        btraitmap_add(&existing, other._traits[i]);
    }
    return existing;
}

btraitmap_t s_btraitmap(void) {
    btraitmap_t list;
    btraitmap_new_n(&list, NULL, 0);
    return list;
}

btraitmap_t s_btraitmap_n(btrait_t **traits, size_t len) {
    btraitmap_t map;
    btraitmap_new_n(&map, traits, len);
    return map;
}

btrait_id_t btrait_newid(void) {
    static btrait_id_t idx = 0;
    return idx++;
}


btrait_t* bobj_require_trait(btrait_c *trait_c, bobj_c* classty) {
    btrait_t *trait = btrait_get(classty, trait_c->id);
    if(!trait) {
        bobj_panic(
            "Type %s must implement trait %s",
            classty->name,
            trait_c->super.name
        );
    }
    return trait;
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
