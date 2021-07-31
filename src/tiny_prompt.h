/**
 * @file tiny_prompt.h
 * @date 31 July 2021
 * @authors vohuutai27@gmail.com
 *
 * @brief tiny_prompt public inteface
 */

#ifndef __TINY_PROMPT_H__
#define __TINY_PROMPT_H__

#define PI_PICO     1

#ifdef ardunio
typedef tinyP_serial_t Serial;
#else // default linux
typedef int tinyP_serial_t ;
#endif

typedef struct function_map{
  const char *name;
  void (*func)(void);
}function_map_t;

/**
 * Tiny prompt init function, must call one when setup
 */
void tiny_prompt_init(tinyP_serial_t serial, size_t func_num);

/**
 * Handling all tiny prompt progess, must call in while(true) loop.
 */
void tiny_prompt_execute(const unsigned char serial_char_data);

#endif // __TINY_PROMPT_H__
