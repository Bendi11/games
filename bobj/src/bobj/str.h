#pragma once

#include "bobj.h"
#include <stdint.h>

typedef struct bstr_t bstr_t;
typedef struct bstr_c {
    bobj_c super;
    char* (*cstr)(bstr_t*);
    void (*append)(bstr_t*,bstr_t*);
    void (*appendc)(bstr_t*, char);
    void (*appendbuf)(bstr_t*, const char*, size_t);
    void (*appendcstr)(bstr_t*, const char*);
} bstr_c;

char* bstr_cstr(bstr_t* self);
void bstr_append(bstr_t* self, bstr_t* other);
void bstr_appendc(bstr_t* self, char c);
void bstr_appendbuf(bstr_t* self, const char * data, size_t len);
void bstr_appendcstr(bstr_t* self, const char *data);

/**
 * Class containing an expandable buffer of characters with interoperobility
 * with C-style NULL-terimination
 */
typedef struct bstr_t {
    bobj_t super;
    char *data;
    uint32_t len;
    uint32_t cap;
} bstr_t;

extern bstr_c* (*bstr_c_impl)(void);
void bstr_new(bstr_t* self);
bstr_t s_bstr(void);
bstr_t* h_bstr(void);
