#ifndef TOYJS_COMMON_H
#define TOYJS_COMMON_H

#define countof(x) (sizeof(x) / sizeof((x)[0]))

#ifdef DEBUG
#define LOG_DEBUG(...) printf("DEBUG: [%s:%d] " __VA_ARGS__, __FILE__, __LINE__)
#define LOG_INFO(...) printf("INFO: [%s:%d] " __VA_ARGS__, __FILE__, __LINE__)
#define LOG_ERROR(...) fprintf(stderr, "ERROR: [%s:%d] " __VA_ARGS__, __FILE__, __LINE__)
#else
#define LOG_DEBUG(...)
#define LOG_INFO(...)
#define LOG_ERROR(...) fprintf(stderr, "ERROR: " __VA_ARGS__)
#endif

typedef struct timer_context_struct_s
{
    JSContext *ctx;
    JSValue func;
} timer_context_struct_t;
// typedef struct JS_RUNTIME

#endif