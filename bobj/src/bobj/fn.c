#include "fn.h"
#include "bobj.h"
#include "bobj/iter/map.h"
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

bobj_t *bfn_call(bfn_t *fn, blist_t *args) {
    if(!vft_cast(bfn_c,fn)->typecheck(fn, args)) {
        blist_iter_t args_iter = blist_iter(args);
        bfnptr_fn_t args_tostr;
        bfnptr_fn_new(&args_tostr, bobj_name, NULL);
        bobj_panic(
            "Cannot call function of class %s with argument types (%s",
            bobj_name((bobj_t*)fn),
            biter_map(
                &args_iter,
             
            )
        );
    }
}

static void bfnptr_fn_drop(bobj_t* fn) {}
static bobj_t* bfnptr_fn_call(bfn_t *fn, blist_t* args) {
    bfnptr_fn_t *self = (bfnptr_fn_t*)fn;
    return self->fn(args);
}
static bool bfnptr_fn_check(bfn_t *fn, blist_t* args) {
    bfnptr_fn_t *self = (bfnptr_fn_t*)fn;
    return self->check ? self->check(args) : true;
}

vft_creator(
    bfnptr_fn_c,
    bfnptr_fn_c_impl,
    (bfnptr_fn_c){
        .super = (bfn_c){
            .super = (bobj_c){
                .name = "bfnptr_fn",
                .size = sizeof(bfnptr_fn_t) - sizeof(bfnptr_fn_c*),
                .drop = bfnptr_fn_drop,
            },
            .typecheck = bfnptr_fn_check,
            .call = bfnptr_fn_call,
        }
    }
)


void bfnptr_fn_new(bfnptr_fn_t *fn, bobj_t* (*fnptr)(blist_t*), bool (*check)(blist_t*)) {
    vft_cast(bfnptr_fn_c, fn) = bfnptr_fn_c_impl();
    fn->fn = fnptr;
    fn->check = check;
}
