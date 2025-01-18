#ifndef TOYJS_COMMON_H
#define TOYJS_COMMON_H

#define countof(x) (sizeof(x) / sizeof((x)[0]))

typedef struct timer_context_struct_s
{
    JSContext *ctx;
    JSValue func;
} timer_context_struct_t;
// typedef struct JS_RUNTIME

#endif