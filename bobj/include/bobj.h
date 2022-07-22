#pragma once

#include <stddef.h>
#include <stdint.h>

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

#define btraitmap_create(...) s_btraitmap_n((__VA_ARGS__), (sizeof(__VA_ARGS__) / sizeof((__VA_ARGS__)[0])))
#define vft_creator(ty, fn_name, ...)                                             \
static ty _BOBJ_EXPAND(_BOBJ_EXPAND(__, ty), __LINE__);                           \
ty* fn_name##_populate(void);                                                     \
ty* (*fn_name)(void) = fn_name##_populate;                                        \
ty* fn_name##_get(void) { return &_BOBJ_EXPAND(_BOBJ_EXPAND(__, ty), __LINE__); } \
ty* fn_name##_populate(void) {                                                    \
    _BOBJ_EXPAND(_BOBJ_EXPAND(__, ty), __LINE__) = __VA_ARGS__;                   \
    fn_name = fn_name##_get;                                                      \
    return &_BOBJ_EXPAND(_BOBJ_EXPAND(__, ty), __LINE__);                         \
}

/** Check if the given pointer to an object is an instance of the given class */
#define bobj_instanceof(ty, ...) (((void*)ty) == (*(void**)(__VA_ARGS__)))

typedef struct btrait_t btrait_t;

typedef struct btraitmap_t {
    btrait_t **_traits;
    uint32_t _len;
    uint32_t _occupied;
} btraitmap_t;

typedef struct bobj_t bobj_t;
typedef struct bobj_c {
    /** \brief Free any memory associated with this object and cleanup all resources */
    void (*drop)(bobj_t*);
    /** \brief Size of an instance of this class in bytes */
    size_t size;
    /** \brief Name of this class */
    char *name;
    /** \brief List of all traits that this class implements */
    btraitmap_t traits;
} bobj_c;

/** \brief Default bobject implementation with purely virtual methods */
extern bobj_c* (*bobj_c_impl)(void);

/**
 * Base class for all objects in the object system
 */
typedef struct bobj_t {
    bobj_c *vft;
} bobj_t;

/** \brief Run the destructor of the given object */
void bobj_drop(bobj_t * const obj);
/** \brief Get the size of the given object */
size_t bobj_size(bobj_t *obj);

/** \brief Get the name of the given object */
const char* bobj_name(bobj_t *obj);

typedef uint64_t btrait_id_t;

/** 
 * \brief Class that all traits objects extend, defining the ID of the trait
 * \extends bobj_c
 */
typedef struct btrait_c {
    bobj_c super;
    /** Generated identifier for this trait */
    btrait_id_t id;
} btrait_c;

extern btrait_c* (*bifmt_c_impl)(void);

/**
 * \brief Base class for all trait objects,
 * as traits themselves are classes and trait implementations are
 * instances of the trait class
 * \extends bobj_t
 */
typedef struct btrait_t {
    bobj_t super;
    /** Offset into the class structure that this trait is implemented in */
    size_t offset;
} btrait_t;

/** \brief Get a pointer to an instance of a btrait_t for the given trait in the given class type */
btrait_t* btrait_get(bobj_c *classty, btrait_id_t trait);

/** \brief Append the given trait objects onto an existing trait list, returning the created list */
btraitmap_t btraitmap_combine(btraitmap_t existing, btraitmap_t other);
void btraitmap_new_n(btraitmap_t* map, btrait_t **traits, size_t len);
btraitmap_t s_btraitmap(void);
btraitmap_t s_btraitmap_n(btrait_t **traits, size_t len);

/** 
 * \brief Function used to mark a class method as virtual, i.e. it must be overriden by a base class. 
 * Any call to this method will result in a panic with a short message written to stderr
 */
BOBJ_NORETURN void bobj_virtual();

/** Default drop function that does nothing */
void bobj_default_drop(bobj_t *);

/** \brief Panic and exit the program after displaying the given message */
BOBJ_FORMAT(printf, 1, 2)
BOBJ_NORETURN
void bobj_panic(const char * message, ...);

/** \brief Require the passed class to implement the passed trait */
btrait_t* bobj_require_trait(btrait_c *trait, bobj_c*);

/** \brief Generate a new unique identifier to be used as a trait's ID */
btrait_id_t btrait_newid(void);
