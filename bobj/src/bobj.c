#include "bobj.h"

#include <malloc.h>
#include <stdarg.h>
#include <stdlib.h>

void bobj_drop(bobj_t *obj) {
    vft_cast(bobj_c, obj)->drop(obj);
}

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
