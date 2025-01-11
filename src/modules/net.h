// ...existing code...
#ifndef JS_NET_MODULE_H
#define JS_NET_MODULE_H
#include "quickjs.h"


typedef struct context_struct_s
{
    JSContext *ctx;
    const char *url;
    JSValue resole_fun;
    JSValue reject_fun;
} fetch_context_struct_t;

// ...existing code...
JSModuleDef *js_init_module_net(JSContext *ctx, const char *module_name);
// ...existing code...
#endif
