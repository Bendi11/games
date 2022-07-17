#pragma once

#include "bobj/fn.h"
#include "bobj/str.h"

typedef struct blogmsg_t blogmsg_t;
typedef struct blogmsg_c {
    bobj_c super;
} blogmsg_c;


typedef struct blogmsg_t {
    bobj_t super;
    bstr_t msg;
} blogmsg_t;


typedef struct bloghook_t bloghook_t;
typedef struct bloghook_c {
    bfn_c super;
    void (*hook)(bloghook_t*,blogmsg_t*);
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
