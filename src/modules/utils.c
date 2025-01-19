#include "quickjs.h"
#include "../bundles/module.c"

typedef struct
{
    const char *name;
    const uint8_t *data;
    uint32_t data_size;
} builtin_js_t;

static builtin_js_t builtins[] = {
    {"toyjsruntime:jsmodules", qjsc_module, qjsc_module_size}};

JSModuleDef *js_cutome_module_loader(JSContext *ctx,
                                     const char *module_name, void *opaque)
{
    builtin_js_t *item = NULL;
    for (int i = 0; i < sizeof(builtins) / sizeof(builtin_js_t); i++)
    {
        if (strcmp(module_name, builtins[i].name) == 0)
        {
            item = &builtins[i];
            break;
        }
    }
    if (item != NULL)
    {
        JSValue obj = JS_ReadObject(ctx, item->data, item->data_size, JS_READ_OBJ_BYTECODE);
        if (JS_IsException(obj))
        {
            JS_ThrowReferenceError(ctx, "Error loading module %s\n", module_name);
            JS_FreeValue(ctx, obj);
            return NULL;
        }

        if (JS_VALUE_GET_TAG(obj) != JS_TAG_MODULE)
        {
            JS_ThrowReferenceError(ctx, "loaded %s, butis not a modules\n", module_name);
            JS_FreeValue(ctx, obj);
            return NULL;
        }
        JSModuleDef *m = JS_VALUE_GET_PTR(obj);
        JS_FreeValue(ctx, obj);
        return m;
    }

    return js_module_loader(ctx, module_name, opaque);
}