#include "bobj/fn.h"
#include "bobj.h"
#include "bobj/iter/map.h"
#include "bobj/list.h"
#include "bobj/ops.h"
#include "bobj/str.h"
#include <malloc.h>
#include <stdarg.h>
#include <string.h>

vft_creator(
    bfn_c,
    fn_c_impl,
    *self_class = (bfn_c){
        .call = (void*)bobj_virtual,
        .typecheck = (void*)bobj_virtual,
        .super = (bobj_c){
            .drop = bobj_virtual,
            .name = "fn",
            .size = 0,
        }
    };
)

bool bfn_typecheck(bfn_t *fn, blist_t *args) { return vft_cast(bfn_c, fn)->typecheck(fn, args); }
bobj_c* bfn_return_ty(bfn_t *fn) { return fn->return_ty; }

static void to_str(blist_t* args, bobj_t* res) {
    const char *name = bobj_name(blist_at(args, 0));
    bstr_t *str = (bstr_t*)res;
    bstr_new(str);
    bstr_appendbuf(str, name, strlen(name));
    bstr_appendc(str, ',');
    bstr_appendc(str, ' ');
}

static bool to_str_check(blist_t* args) {
    return blist_len(args) == 1;
}

static void foldstr(blist_t *args, bobj_t *res) {
    bstr_t *acc = (bstr_t*)blist_at(args, 0);
    bstr_t *arg = (bstr_t*)blist_at(args, 1);
    bstr_append(acc, arg);
}

static bool foldstr_check(blist_t *args) {
    return blist_len(args) == 2;
}

void bfn_call(bfn_t *fn, blist_t *args, bobj_t *res) {
    if(!vft_cast(bfn_c,fn)->typecheck(fn, args)) {
        blist_iter_t args_iter = s_blist_iter(args);
        bsingle_list_t tostr_params = s_bsingle_list((bobj_t*)h_bobj_class(bobj_c_impl()));
        bfnptr_fn_t args_tostr = s_bfnptr_fn(to_str, (blist_t*)&tostr_params, (bobj_c*)bstr_c_impl());

        bfnptr_fn_t tostr_fold = s_bfnptr_fn(foldstr, foldstr_check, NULL);
        bstr_t str = s_bstr();
        biter_map_t tostr_map = s_biter_map((biter_t*)&args_iter, (bfn_t*)&args_tostr);
        biter_fold(
            (biter_t*)&tostr_map,
            (bobj_t*)&str,
            (bfn_t*)&tostr_fold
        );

        bobj_panic(
            "Cannot call function of class %s with arguments (%s)\n",
            bobj_name((bobj_t*)fn),
            bstr_cstr(&str)
        );
    } else {
        vft_cast(bfn_c, fn)->call(fn, args, res);
        if(res != NULL && !bobj_instanceof(fn->return_ty, res)) {
            bobj_panic(
                "Function of type %s returned a value of type %s when a return type of %s was declared",
                bobj_name((bobj_t*)fn),
                bobj_name((bobj_t*)res),
                fn->return_ty->name
            );
        }
    }
}

static void bfnptr_fn_drop_impl(bobj_t* fn) {}
static void bfnptr_fn_call_impl(bfn_t *fn, blist_t* args, bobj_t* res) {
    bfnptr_fn_t *self = (bfnptr_fn_t*)fn;
    self->fn(args, res);
}
static bool bfnptr_fn_check_impl(bfn_t *fn, blist_t* args) {
    bfnptr_fn_t *self = (bfnptr_fn_t*)fn;
    return self->check ? bieq_eq((bobj_t*)self->check, (bobj_t*)args) : true;
}

vft_creator(
    bfnptr_fn_c,
    bfnptr_fn_c_impl,
    *self_class = (bfnptr_fn_c){
        .super = (bfn_c){
            .super = (bobj_c){
                .name = "bfnptr_fn",
                .size = sizeof(bfnptr_fn_t) - sizeof(bfnptr_fn_c*),
                .drop = bfnptr_fn_drop_impl,
            },
            .typecheck = bfnptr_fn_check_impl,
            .call = bfnptr_fn_call_impl,
        }
    };
)


void bfnptr_fn_new(bfnptr_fn_t *fn, bfnptr_fn_call_t fnptr, blist_t *params, bobj_c *return_ty) {
    vft_cast(bfnptr_fn_c, fn) = bfnptr_fn_c_impl();
    fn->fn = fnptr;
    fn->check = params;
    fn->super.return_ty = return_ty;
}
bfnptr_fn_t* h_bfnptr_fn(bfnptr_fn_call_t ptr, blist_t *params, bobj_c *return_ty) {
    bfnptr_fn_t *fn = malloc(sizeof(*fn));
    bfnptr_fn_new(fn, ptr, params, return_ty);
    return fn;
}
bfnptr_fn_t s_bfnptr_fn(bfnptr_fn_call_t ptr, blist_t *params, bobj_c *return_ty) {
    bfnptr_fn_t fn;
    bfnptr_fn_new(&fn, ptr, params, return_ty);
    return fn;
}
