#pragma once

#include "bobj/fn.h"


typedef struct bloghook_t bloghook_t;
typedef struct bloghook_c {
    bfn_c super;
} bloghook_c;

/** 
 * Abstract base class allowing for user-defined logging hooks to be defined and called
 * when logging methods are called
 */
typedef struct bloghook_t {
    bfn_t super;
} bloghook_t;

typedef struct blogger_t blogger_t;
typedef struct blogger_c {
    bobj_c super;
} blogger_c;

extern blogger_c* (*blogger_c_impl)(void);

/** 
 * Concrete base class, a singleton instance of which is stored as the 
 * program's logger 
 */
typedef struct blogger_t {
    bobj_t super;
} blogger_t;
