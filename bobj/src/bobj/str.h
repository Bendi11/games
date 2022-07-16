#pragma once

#include "bobj.h"
#include <stdint.h>

typedef struct bstr_t bstr_t;
typedef struct bstr_c {
    bobj_c super;
    char* (*cstr)(bstr_t*);
    void (*append)(bstr_t*,bstr_t*);
    void (*appendc)(bstr_t*, char);
    void (*appendcstr)(bstr_t*, const char*, size_t);
} bstr_c;

inline char* bstr_cstr(bstr_t* self) { return vft_cast(bstr_c, self)->cstr(self); }
inline void bstr_append(bstr_t* self, bstr_t* other) { return vft_cast(bstr_c, self)->append(self, other); }
inline void bstr_appendc(bstr_t* self, char c) { return vft_cast(bstr_c, self)->appendc(self, c); }
inline void bstr_appendcstr(bstr_t* self, const char * data, size_t len) { return vft_cast(bstr_c, self)->appendcstr(self, data, len); }

typedef struct bstr_t {
    bobj_t super;
    char *data;
    uint32_t len;
    uint32_t cap;
} bstr_t;

extern bstr_c* (*bstr_c_impl)(void);
void bstr_new(bstr_t* self);
inline bstr_t s_bstr(void) {
    bstr_t str;
    bstr_new(&str);
    return str;
}
bstr_t* h_bstr(void);
