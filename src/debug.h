#ifndef __DEBUG_H__
#define __DEBUG_H__

#define DEBUG 1

#if (DEBUG == 1)

#define debug_print(...) \
    do{ printf("%s:%d ", __FILE__, __LINE__); \
        printf(__VA_ARGS__); } while(0)

void func() {
    debug_print("a: %d\n", 5);
}

#else
#define debug_print(...)
#endif

#endif // __DEBUG_H__
