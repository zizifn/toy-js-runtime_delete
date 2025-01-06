#include <stdlib.h>
#include <curl/curl.h>

#include "quickjs.h"


#define countof(x) (sizeof(x) / sizeof((x)[0]))

static JSValue js_ping(JSContext *ctx, JSValue this_val, int argc, JSValue *argv) {
    return JS_NewString(ctx, "pong");
}

static JSClassID js_response_class_id;

static JSValue js_new_response(JSContext *ctx, const char *body, size_t body_len, 
                             long status_code, const char *status_text,
                             struct curl_slist *headers) {
    JSValue response_proto, response_obj;
    
    // Get Response prototype
    response_proto = JS_GetPropertyStr(ctx, JS_GetGlobalObject(ctx), "Response");
    if (JS_IsException(response_proto))
        return JS_EXCEPTION;
        
    response_obj = JS_NewObjectProtoClass(ctx, response_proto, js_response_class_id);
    
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
    
    return response_obj;
}

// curl

struct MemoryStruct {
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
                          int argc, JSValueConst *argv) {
    JSValue promise, resolving_funcs[2];
    CURL *curl;
    struct curl_response {
        char *data;
        size_t size;
    } response = { NULL, 0 };
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
    if (!curl) {
        JS_FreeValue(ctx, resolving_funcs[0]);
        JS_FreeValue(ctx, resolving_funcs[1]);
        goto fail;
    }

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    res = curl_easy_perform(curl);
    
    if (res == CURLE_OK) {
        // Success - resolve promise with response data
        JSValue response_str = JS_NewStringLen(ctx, response.data, response.size);
        JS_Call(ctx, resolving_funcs[0], JS_UNDEFINED, 1, &response_str);
    } else {
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

static const JSCFunctionListEntry js_ping_funcs[] = {
    JS_CFUNC_DEF("ping", 0, js_ping),
    JS_CFUNC_DEF("fetch", 0, js_fetch)
};

static int js_net_init(JSContext *ctx, JSModuleDef *m) {
    return JS_SetModuleExportList(ctx, m, js_ping_funcs, countof(js_ping_funcs));
}

JSModuleDef *js_init_module_net(JSContext *ctx, const char *module_name) {
    JSModuleDef *m = JS_NewCModule(ctx, module_name, js_net_init);
    if (!m)
        return NULL;
    JS_AddModuleExportList(ctx, m, js_ping_funcs, countof(js_ping_funcs));
    return m;
}
