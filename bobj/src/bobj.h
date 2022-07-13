#pragma once

#define vft_cast(ty, ...) (*( ty **) __VA_ARGS__ )

typedef struct obj_t obj_t;
typedef struct obj_i {
    /** \brief Free any memory associated with this object and cleanup all resources */
    void (*drop)(obj_t*);
} obj_i;

typedef struct obj_t {
    obj_i *vft;
} obj_t;

void drop(obj_t *obj);
