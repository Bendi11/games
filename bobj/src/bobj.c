#include "bobj.h"

#include <malloc.h>
#include <stdarg.h>
#include <stdlib.h>

vft_creator(
    bobj_c,
    bobj_c_impl,
    (bobj_c){
        .size = 0,
        .drop = bobj_virtual,
        .name = "bobj"
    }
)

void bobj_drop(bobj_t *obj) {
    vft_cast(bobj_c, obj)->drop(obj);
}

size_t bobj_size(bobj_t *obj) { return vft_cast(bobj_c, obj)->size; }
const char* bobj_name(bobj_t *obj) { return vft_cast(bobj_c, obj)->name; }

BOBJ_NORETURN void bobj_virtual() {
    bobj_panic("Attempt to invoke a virtual method");
}

void bobj_panic(const char *message, ...) {
    va_list list;
    va_start(list, message);
    
    vfprintf(stderr, message, list);
    fputs("", stderr);
    va_end(list);
    exit(-1);
}
