#include <stdlib.h>

#include "quickjs.h"
#include "quickjs-libc.h"

#include "uv.h"

#include "common.h"

// setTimeout(() => {
//     console.log("Set time out");
// }, 10)

void js_os_setTimeout_cb(uv_timer_t *handle)
{
    int r;
    JSValue ret, func;
    LOG_DEBUG("js_os_setTimeout_cb\n");
    timer_context_struct_t *context = (timer_context_struct_t *)handle->data;
    // func = JS_DupValue(context->ctx, context->func);
    ret = JS_Call(context->ctx, context->func, JS_UNDEFINED, 0, NULL);
    if (JS_IsException(ret))
    {
        LOG_ERROR("js_os_setTimeout_cb exception\n");
        js_std_dump_error(context->ctx);
        exit(1);
    }
    
    LOG_DEBUG("js_os_setTimeout_cb res: %s \n", JS_ToCString(context->ctx, ret));
    JS_FreeValue(context->ctx, ret);
    free(context);
}

static JSValue js_os_setTimeout(JSContext *ctx, JSValue this_val,
                                int argc, JSValue *argv, int magic)
{
  
    JSValue func = argv[0];
    if (!JS_IsFunction(ctx, func)){
        return JS_ThrowTypeError(ctx, "not a function");
    }
  /*
    JSValue ret = JS_Call(ctx, func, JS_UNDEFINED, 0, NULL);
    if (JS_IsException(ret))
    {
        js_std_dump_error(ctx);
        exit(1);
    }
    return ret;*/


    timer_context_struct_t *timer_context = malloc(sizeof(timer_context_struct_t));
    timer_context->ctx = ctx;
    timer_context->func = JS_DupValue(ctx, func);


    uv_timer_t *timer = malloc(sizeof(uv_timer_t));
    uv_timer_init(uv_default_loop(), timer);
    timer->data = timer_context;
    uv_timer_start(timer, js_os_setTimeout_cb, 1000, 0);
    // JS_FreeValue(ctx, func);
    return JS_NewInt32(ctx, 1);
}

static const JSCFunctionListEntry js_global_funcs[] = {
    JS_CFUNC_MAGIC_DEF("setTimeout", 2, js_os_setTimeout, 0),
    // JS_CFUNC_MAGIC_DEF("setInterval", 2, js_os_setTimeout, 1 ),
    // JS_CFUNC_DEF("clearTimeout", 1, js_os_clearTimeout ),
    // JS_CFUNC_DEF("clearInterval", 1, js_os_clearTimeout ),
};
void js_global_add_helpers(JSContext *ctx, int argc, char **argv)
{
    JSValue global_obj;
    global_obj = JS_GetGlobalObject(ctx);

    JS_SetPropertyFunctionList(ctx, global_obj, js_global_funcs, countof(js_global_funcs));
    JS_FreeValue(ctx, global_obj); // Free the global_obj to allow garbage collection
}