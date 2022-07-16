#pragma once

#include <stddef.h>

#if defined(__clang__) || defined(__GNUC__)
#define BOBJ_READONLY(...) __attribute__((access(read_only, __VA_ARGS__)))
#define BOBJ_NORETURN __attribute__((__noreturn__))
#define BOBJ_FORMAT(...) __attribute__((format(__VA_ARGS__)))
#else
#define BOBJ_READONLY(...)
#define BOBJ_NORETURN
#define BOBJ_FORMAT
#endif


/**
 * Cast a pointer to an instance of a class to a pointer to a class structure
 * that the instance is
 */
#define vft_cast(ty, ...) (*( ty **)(__VA_ARGS__))

#define _BOBJ_CONCAT(x, y) x##y
#define _BOBJ_EXPAND(x, y) _BOBJ_CONCAT(x, y)

#define vft_creator(ty, fn_name, ...)                                            \
static ty * _BOBJ_EXPAND(_BOBJ_EXPAND(__, ty), __LINE__) = NULL;                 \
ty* fn_name##_populate(void);                                                    \
ty* (*fn_name)(void) = fn_name##_populate;                                       \
ty* fn_name##_get(void) { return _BOBJ_EXPAND(_BOBJ_EXPAND(__, ty), __LINE__); } \
ty* fn_name##_populate(void) {                                                   \
    _BOBJ_EXPAND(_BOBJ_EXPAND(__, ty), __LINE__) = malloc(sizeof(ty));           \
    *_BOBJ_EXPAND(_BOBJ_EXPAND(__, ty), __LINE__) = __VA_ARGS__;                 \
    fn_name = fn_name##_get;                                                     \
    return _BOBJ_EXPAND(_BOBJ_EXPAND(__, ty), __LINE__);                         \
}

/** Check if the given pointer to an object is an instance of the given class */
#define bobj_instanceof(ty, ...) (((void*)ty) == (*(void**)__VA_ARGS__))

typedef struct bobj_t bobj_t;
typedef struct bobj_c {
    /** \brief Free any memory associated with this object and cleanup all resources */
    void (*drop)(bobj_t*);
    /** \brief Size of an instance of this class in bytes */
    size_t size;
    /** \brief Name of this class */
    char *name;
} bobj_c;

/**
 * Base class for all objects in the object system
 */
typedef struct bobj_t {
    bobj_c *vft;
} bobj_t;

/** \brief Run the destructor of the given object */
void bobj_drop(bobj_t * const obj);

inline size_t bobj_size(bobj_t *obj) { return vft_cast(bobj_c, obj)->size; }

/** \brief Get the name of the given object */
inline const char* bobj_name(bobj_t *obj) { return vft_cast(bobj_c, obj)->name; }

/** 
 * \brief Function used to mark a class method as virtual, i.e. it must be overriden by a base class. 
 * Any call to this method will result in a panic with a short message written to stderr
 */
BOBJ_NORETURN void bobj_virtual();
/** \brief Panic and exit the program after displaying the given message */
BOBJ_FORMAT(printf, 1, 2)
BOBJ_NORETURN
void bobj_panic(const char * message, ...);
