#include <stdio.h>
#include <stdlib.h>
#include "uv.h"
#include "quickjs.h"
#include "quickjs-libc.h"

#include "modules/test.h"
#include "modules/net.h"
#include "modules/global.h"

#include "cutils.h"
#include "modules/utils.h"
#include "modules/quickjs-ffi.h"

// Add console object setup

static int eval_buf(JSContext *ctx, const void *buf, int buf_len,
                    const char *filename, int eval_flags)
{
    JSValue val;
    int ret;

    if ((eval_flags & JS_EVAL_TYPE_MASK) == JS_EVAL_TYPE_MODULE)
    {
        /* for the modules, we compile then run to be able to set
           import.meta */
        val = JS_Eval(ctx, buf, buf_len, filename,
                      eval_flags | JS_EVAL_FLAG_COMPILE_ONLY);
        if (!JS_IsException(val))
        {
            js_module_set_import_meta(ctx, val, 1, 1);
            val = JS_EvalFunction(ctx, val);
        }
        val = js_std_await(ctx, val);
    }
    else
    {
        val = JS_Eval(ctx, buf, buf_len, filename, eval_flags);
    }
    if (JS_IsException(val))
    {
        js_std_dump_error(ctx);
        ret = -1;
    }
    else
    {
        ret = 0;
    }
    JS_FreeValue(ctx, val);
    return ret;
}

static int eval_file(JSContext *ctx, const char *filename, int module)
{
    uint8_t *buf;
    int ret, eval_flags;
    size_t buf_len;

    buf = js_load_file(ctx, &buf_len, filename);
    if (!buf)
    {
        perror(filename);
        exit(1);
    }

    if (module < 0)
    {
        module = (js__has_suffix(filename, ".mjs") ||
                  JS_DetectModule((const char *)buf, buf_len));
    }
    if (module)
        eval_flags = JS_EVAL_TYPE_MODULE;
    else
        eval_flags = JS_EVAL_TYPE_GLOBAL;
    ret = eval_buf(ctx, buf, buf_len, filename, eval_flags);
    js_free(ctx, buf);
    return ret;
}

static JSValue js_gc(JSContext *ctx, JSValue this_val,
                     int argc, JSValue *argv)
{
    JS_RunGC(JS_GetRuntime(ctx));
    return JS_UNDEFINED;
}

static const JSCFunctionListEntry global_obj[] = {
    JS_CFUNC_DEF("gc", 0, js_gc),
#if defined(__ASAN__) || defined(__UBSAN__)
    JS_PROP_INT32_DEF("__running_with_sanitizer__", 1, JS_PROP_C_W_E),
#endif
};

void cleanup(JSRuntime *rt, JSContext *ctx)
{
    js_std_free_handlers(rt);
    JS_FreeContext(ctx);
    JS_FreeRuntime(rt);
}

static uv_check_t check_handle;
static uv_prepare_t prepare_handle;
static uv_idle_t idle_handle;
static JSContext *g_ctx; // store a reference to use in callback

// execute promise job in check phare is not quite right. but anyway, we just TOY runtime.
// https://github.com/saghul/txiki.js/issues/684
static void check_cb(uv_check_t *handle) {
    // printf("check_cb------");

    JSContext *ctx1;
    int err;
    while (1) {
        err = JS_ExecutePendingJob(JS_GetRuntime(g_ctx), &ctx1);
        printf("check_cb------err is %d\n", err);
        if (err <= 0) {
            // if err < 0, an exception occurred
            break;
        }
    }

    //
    if(JS_IsJobPending(JS_GetRuntime(g_ctx))){
         printf("check_cb------have JobPending\n");
    }else{
        uv_idle_stop(&idle_handle);
    }
    
}

static void prepare_cb(uv_prepare_t* handle) {
    // printf("prepare_cb\n");
    // // Do nothing, just ensure the loop triggers uv_check
    //     JSContext *ctx1;
    // int err;
    // while (1) {
    //     err = JS_ExecutePendingJob(JS_GetRuntime(g_ctx), &ctx1);
    //     printf("prepare_cb------err is %d\n", err);
    //     if (err <= 0) {
    //         // if err < 0, an exception occurred
    //         break;
    //     }
    // }
}

void idle_cb(uv_idle_t *handle)
{
    // JSRuntime *rt = handle->data;
    // if(JS_IsJobPending(rt)){
    //     //noop
    //     printf("idle_cb have JS_IsJobPending");
    // }else{
    //     printf("idle_cb have not JS_IsJobPending");
    //     uv_idle_stop(handle);
    // }

    printf("idle_cb\n");
    // noop
}

int main(int argc, char **argv)
{
    printf("toy js runtime example\n");
    if (argc != 2)
    {
        printf("Usage: %s <javascript-file>\n", argv[0]);
        return 1;
    }

    JSRuntime *rt;
    JSContext *ctx;
    JSValue val;

    rt = JS_NewRuntime();
    if (!rt)
    {
        fprintf(stderr, "Failed to create JS runtime\n");
        return 1;
    }

    ctx = JS_NewContext(rt);
    if (!ctx)
    {
        JS_FreeRuntime(rt);
        fprintf(stderr, "Failed to create JS context\n");
        return 1;
    }

    // Initialize standard handlers
    js_std_init_handlers(rt);
    

    JS_SetModuleLoaderFunc(rt, NULL, js_cutome_module_loader, NULL);

    // Add console and other helpers
    js_init_module_std(ctx, "toyjsruntime:std");
    js_init_module_os(ctx, "toyjsruntime:os");
    js_std_add_helpers(ctx, argc, argv);
    js_global_add_helpers(ctx, argc, argv);

    // add test module
    js_init_module_test(ctx, "toyjsruntime:test");
    js_init_module_ffi(ctx, "toyjsruntime:ffi");


    // add test module
    js_init_module_net(ctx, "toyjsruntime:net");

    g_ctx = ctx;

    uv_prepare_init(uv_default_loop(), &prepare_handle);
    uv_prepare_start(&prepare_handle, prepare_cb);
    uv_unref((uv_handle_t *)&prepare_handle);

    uv_check_init(uv_default_loop(), &check_handle);
    uv_check_start(&check_handle, check_cb);
    uv_unref((uv_handle_t *)&check_handle);

    uv_idle_init(uv_default_loop(), &idle_handle);
    uv_idle_start(&idle_handle, idle_cb);
    // idle_handle.data = rt;

    // Pass 1 to eval_file to enable module mode
    if (eval_file(ctx, argv[1], 1))
    {
        printf("eval_file failed\n");
        cleanup(rt, ctx);
        return 1;
    }
    int r;
    printf("JS_IsJobPending is %d \n", JS_IsJobPending(rt));
    // do {
    //     printf("uv_run\n");
    //     // uv__maybe_idle(qrt);
    //     uv_idle_start(&idle_handle, idle_cb);
    //     r = uv_run(uv_default_loop(), UV_RUN_DEFAULT);
    // } while (r == 0 && JS_IsJobPending(rt));
    uv_run(uv_default_loop(), UV_RUN_DEFAULT);

    printf("JS_IsJobPending is %d \n", JS_IsJobPending(rt));
    // Run event loop to process any pending jobs (promises, etc)
    // js_std_loop(ctx);

    JS_FreeValue(ctx, val);

    // Cleanup
    cleanup(rt, ctx);
    return 0;
}
