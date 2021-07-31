/**
 * @file debug.h
 * @date 31 July 2021
 * @authors vohuutai27@gmail.com
 *
 * @brief implement debug function
 */

#ifndef __DEBUG_H__
#define __DEBUG_H__

# define _TINY_PROMPT_DBG_      0

#if (_TINY_PROMPT_DBG_ == 1)

#define debug_print(...) \
    do{ printf("\n%s:%d ", __FILE__, __LINE__); \
        printf(__VA_ARGS__); } while(0)

//void func() {
//    debug_print("a: %d\n", 5);
//}

#else
#define debug_print(...)
#endif

#endif // __DEBUG_H__
