#include "str.h"
#include <string.h>
#include <malloc.h>

static void bstr_drop(bobj_t* obj) {
    bstr_t *self = (bstr_t*)obj;
    free(self->data);
}

static void bstr_resize_impl(bstr_t* str, size_t cap) {
    str->data = realloc(str, cap);
}

static void bstr_appendc_impl(bstr_t* str, char c) {
    if(str->len >= str->cap) {
        str->cap += 10;
        bstr_resize_impl(str, str->cap);    
    }
    str->data[str->len] = c;
    str->len += 1;
}

static void bstr_appendcstr_impl(bstr_t* self, const char* data, size_t len) {
    if(self->len + len >= self->cap) {
        self->cap += len + 10;
        bstr_resize_impl(self, self->cap);
    }

    memcpy(self->data + self->len, data, len);
    self->len += len;
}

static void bstr_append_impl(bstr_t *self, bstr_t *other) {
    bstr_appendcstr_impl(self, other->data, other->len);
}

static char* bstr_c_str_impl(bstr_t* str) {
    bstr_appendc_impl(str, '\0');
    return str->data;
}

vft_creator(
    bstr_c,
    bstr_c_impl,
    (bstr_c){
        .super = (bobj_c){
            .name = "bstr_c",
            .drop = bstr_drop,
            .size = sizeof(bstr_t) - sizeof(bstr_c*),
        },
        .cstr = bstr_c_str_impl,
        .append = bstr_append_impl,
        .appendc = bstr_appendc_impl,
        .appendcstr = bstr_appendcstr_impl,
    }
)

void bstr_new(bstr_t* self) {
    vft_cast(bstr_c, self) = bstr_c_impl();
    self->data = calloc(sizeof(char), 25);
    self->cap = 25;
    self->len = 0;
}

bstr_t* h_bstr() {
    bstr_t* str = malloc(sizeof(*str));
    bstr_new(str);
    return str;
}
