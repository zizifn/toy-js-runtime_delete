#include <stdio.h>
#include <stdlib.h>
#include <uv.h>
#include "quickjs.h"
#include "quickjs-libc.h"

#include "modules/test.h"
#include "modules/net.h"

#include "cutils.h"

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
    

    JS_SetModuleLoaderFunc(rt, NULL, js_module_loader, NULL);

    // Add console and other helpers
    js_init_module_std(ctx, "toyjsruntime:std");
    js_init_module_os(ctx, "toyjsruntime:os");
    js_std_add_helpers(ctx, argc, argv);

    // add test module
    js_init_module_test(ctx, "toyjsruntime:test");

        // add test module
    js_init_module_net(ctx, "toyjsruntime:net");

    // Pass 1 to eval_file to enable module mode
    if (eval_file(ctx, argv[1], 1))
    {
        cleanup(rt, ctx);
        return 1;
    }
    uv_run(uv_default_loop(), UV_RUN_DEFAULT);
    // Run event loop to process any pending jobs (promises, etc)
    js_std_loop(ctx);

    JS_FreeValue(ctx, val);

    // Cleanup
    cleanup(rt, ctx);
    return 0;
}
