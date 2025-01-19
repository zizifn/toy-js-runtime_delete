#ifndef TOYJSRUNTIME_UTILS_MODULE_H
#define TOYJSRUNTIME_UTILS_MODULE_H

#include "quickjs.h"

JSModuleDef *js_cutome_module_loader(JSContext *ctx,
                                     const char *module_name, void *opaque);
#endif