#include <stdlib.h>
#include <curl/curl.h>
#include <uv.h>
#include "net.h"

#include "quickjs.h"

#define countof(x) (sizeof(x) / sizeof((x)[0]))

static JSValue js_ping(JSContext *ctx, JSValue this_val, int argc, JSValue *argv)
{
    return JS_NewString(ctx, "pong");
}

static JSClassID js_response_class_id;

static JSClassDef js_response_class = {
    "Response",
    // ...existing code...
};

static JSValue js_new_response(JSContext *ctx, const char *body, size_t body_len,
                               long status_code, const char *status_text,
                               struct curl_slist *headers)
{
    JSValue global_obj = JS_GetGlobalObject(ctx);
    JSValue response_proto = JS_GetPropertyStr(ctx, global_obj, "Response");
    JS_FreeValue(ctx, global_obj);
    if (JS_IsException(response_proto))
        return JS_EXCEPTION;

    JSValue response_obj = JS_NewObjectProtoClass(ctx, response_proto, js_response_class_id);

    // Set response properties
    JS_DefinePropertyValueStr(ctx, response_obj, "ok",
                              JS_NewBool(ctx, status_code >= 200 && status_code < 300), JS_PROP_C_W_E);
    JS_DefinePropertyValueStr(ctx, response_obj, "status",
                              JS_NewInt32(ctx, status_code), JS_PROP_C_W_E);
    JS_DefinePropertyValueStr(ctx, response_obj, "statusText",
                              JS_NewString(ctx, status_text), JS_PROP_C_W_E);
    JS_DefinePropertyValueStr(ctx, response_obj, "body",
                              JS_NewStringLen(ctx, body, body_len), JS_PROP_C_W_E);

    // Convert curl headers to Headers object
    JSValue headers_obj = JS_NewObject(ctx); // Assuming js_new_headers creates an object
    // Add code to populate headers_obj with headers if necessary
    JS_DefinePropertyValueStr(ctx, response_obj, "headers", headers_obj, JS_PROP_C_W_E);

    JS_FreeValue(ctx, response_proto);
    return response_obj;
}

// curl

struct MemoryStruct
{
    char *memory;
    size_t size;
};

static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)userp;

    char *ptr = realloc(mem->memory, mem->size + realsize + 1);
    mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;

    return realsize;
}

static JSValue js_fetch(JSContext *ctx, JSValueConst this_val,
                        int argc, JSValueConst *argv)
{
    JSValue promise, resolving_funcs[2];
    CURL *curl;
    struct curl_response
    {
        char *data;
        size_t size;
    } response = {NULL, 0};
    CURLcode res;
    const char *url;

    if (!JS_IsString(argv[0]))
        return JS_EXCEPTION;

    url = JS_ToCString(ctx, argv[0]);
    if (!url)
        return JS_EXCEPTION;

    // Create promise and get resolve/reject functions
    promise = JS_NewPromiseCapability(ctx, resolving_funcs);
    if (JS_IsException(promise))
        goto fail;

    curl = curl_easy_init();
    if (!curl)
    {
        JS_FreeValue(ctx, resolving_funcs[0]);
        JS_FreeValue(ctx, resolving_funcs[1]);
        goto fail;
    }

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    res = curl_easy_perform(curl);

    if (res == CURLE_OK)
    {
        // Success - resolve promise with response data
        JSValue response_str = JS_NewStringLen(ctx, response.data, response.size);
        JS_Call(ctx, resolving_funcs[0], JS_UNDEFINED, 1, &response_str);
    }
    else
    {
        // Error - reject promise
        JSValue error = JS_NewString(ctx, curl_easy_strerror(res));
        JS_Call(ctx, resolving_funcs[1], JS_UNDEFINED, 1, &error);
    }

    free(response.data);
    curl_easy_cleanup(curl);
    JS_FreeCString(ctx, url);
    JS_FreeValue(ctx, resolving_funcs[0]);
    JS_FreeValue(ctx, resolving_funcs[1]);
    return promise;

fail:
    JS_FreeCString(ctx, url);
    return JS_EXCEPTION;
}

static void idle_fetch_cb(uv_idle_t *handle)
{
    printf("idle_fetch_cb\n");

    fetch_context_struct_t *fetch_context = (fetch_context_struct_t *)handle->data;
    JSContext *jsctx = fetch_context->ctx;
    CURL *curl;
    struct curl_response
    {
        char *data;
        size_t size;
    } response = {NULL, 0};
    CURLcode res;
    curl = curl_easy_init();

    curl_easy_setopt(curl, CURLOPT_URL, fetch_context->url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    res = curl_easy_perform(curl);

    if (res == CURLE_OK)
    {
        // Success - resolve promise with response data
        JSValue response_str = JS_NewStringLen(jsctx, response.data, response.size);
        JS_Call(jsctx, *fetch_context->resole_fun, JS_UNDEFINED, 1, &response_str);
    }
    else
    {
        // Error - reject promise
        JSValue error = JS_NewString(jsctx, curl_easy_strerror(res));
        JS_Call(jsctx, *fetch_context->reject_fun, JS_UNDEFINED, 1, &error);
    }

    free(response.data);
    curl_easy_cleanup(curl);

    uv_idle_stop(handle);
    JS_FreeCString(jsctx, fetch_context->url);
    JS_FreeValue(jsctx, *fetch_context->reject_fun);
    JS_FreeValue(jsctx, *fetch_context->resole_fun);
    free(fetch_context);
}

static JSValue js_fetch_new(JSContext *ctx, JSValueConst this_val,
                            int argc, JSValueConst *argv)
{
    int r;
    uv_idle_t *idle_handle;
    JSValue promise; 
    JSValue *resolving_funcs = malloc(2 * sizeof(JSValue));
    const char *url;
    fetch_context_struct_t *fetch_context;

    // Create promise and get resolve/reject functions
    promise = JS_NewPromiseCapability(ctx, resolving_funcs);
    if (JS_IsException(promise))
        goto fail;

    if (!JS_IsString(argv[0]))
        return JS_EXCEPTION;

    url = JS_ToCString(ctx, argv[0]);
    if (!url)
        return JS_EXCEPTION;

    fetch_context = malloc(sizeof(fetch_context_struct_t));
    fetch_context->url = url;
    fetch_context->ctx = ctx;
    fetch_context->resole_fun = &resolving_funcs[0];
    fetch_context->reject_fun = &resolving_funcs[1];

    idle_handle = malloc(sizeof(uv_idle_t));
    r = uv_idle_init(uv_default_loop(), idle_handle);
    if (r)
        goto fail;

    idle_handle->data = fetch_context;
    printf("uv_fetch_idle_start\n");
    r = uv_idle_start(idle_handle, idle_fetch_cb);

    return promise;

fail:
    JS_FreeCString(ctx, url);
    return JS_EXCEPTION;
}

static const JSCFunctionListEntry js_ping_funcs[] = {
    JS_CFUNC_DEF("ping", 0, js_ping),
    JS_CFUNC_DEF("old_fetch", 1, js_fetch),
    JS_CFUNC_DEF("fetch", 1, js_fetch_new)};

static int js_net_init(JSContext *ctx, JSModuleDef *m)
{
    return JS_SetModuleExportList(ctx, m, js_ping_funcs, countof(js_ping_funcs));
}

JSModuleDef *js_init_module_net(JSContext *ctx, const char *module_name)
{
    JSModuleDef *m = JS_NewCModule(ctx, module_name, js_net_init);
    if (!m)
        return NULL;
    JS_AddModuleExportList(ctx, m, js_ping_funcs, countof(js_ping_funcs));
    return m;
}
