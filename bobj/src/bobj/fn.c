#include "fn.h"
#include "bobj.h"
#include <malloc.h>
#include <stdarg.h>

vft_creator(
    bfn_c,
    fn_c_impl,
    (bfn_c){
        .call = (void*)bobj_virtual,
        .typecheck = (void*)bobj_virtual,
        .super = (bobj_c){
            .drop = bobj_virtual,
            .name = "fn",
            .size = 0,
        }
    }
)

bobj_t *bfn_call(bfn_t *fn, size_t len, bobj_t *args[]) {
    if(!vft_cast(bfn_c,fn)->typecheck(fn, len, args)) {
        bobj_panic(
            "Cannot call function of class %s with argument types (",
            vft_cast(bobj_c,fn)->name
        );
    }
}
