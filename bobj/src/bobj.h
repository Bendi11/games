#pragma once

#include <stddef.h>

#if __has_attribute(access)
#define BOBJ_READONLY(...) __attribute__((access(read_only, __VA_ARGS__)))
#else
#define BOBJ_READONLY(...)
#endif

/**
 * Cast a pointer to an instance of a class to a pointer to the given vtable
 * that the class extends
 */
#define vft_cast(ty, ...) (*( ty **) __VA_ARGS__ )

#define _BOBJ_CONCAT(x, y) x##y
#define _BOBJ_EXPAND(x, y) _BOBJ_CONCAT(x, y)

#define vft_creator(ty, fn_name, ...)                                      \
static ty * _BOBJ_EXPAND(_BOBJ_EXPAND(__, ty), __LINE__) = NULL;           \
ty* fn_name (void) {                                                       \
    if(_BOBJ_EXPAND(_BOBJ_EXPAND(__, ty), __LINE__) == NULL) {             \
        _BOBJ_EXPAND(_BOBJ_EXPAND(__, ty), __LINE__) = malloc(sizeof(ty)); \
        *_BOBJ_EXPAND(_BOBJ_EXPAND(__, ty), __LINE__) = __VA_ARGS__;       \
    }                                                                      \
    return _BOBJ_EXPAND(_BOBJ_EXPAND(__, ty), __LINE__);                   \
}

typedef struct bobj_t bobj_t;
typedef struct bobj_v {
    /** \brief Free any memory associated with this object and cleanup all resources */
    void (*drop)(bobj_t*);
    /** \brief Size of an instance of this class in bytes */
    size_t size;
} bobj_v;

/**
 * Base class for all objects in the object system
 */
typedef struct bobj_t {
    bobj_v *vft;
} bobj_t;

/** \brief Run the destructor of the given object */
void bobj_drop(bobj_t * const obj);

/**
 * Interface for addable objects
 */
typedef struct add_i {
    bobj_t* (*add)(bobj_t*,bobj_t*);
} add_i;

typedef struct bint_v {
    bobj_v super;
    add_i add_impl;
} bint_v;

/** \brief Get the default vtable for the bint_t type */
bint_v *bint_v_impl();

typedef struct bint_t {
    bobj_t super;
    int val;
} bint_t;

void bint_new(bint_t *, int val);
