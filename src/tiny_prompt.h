
#ifndef __TINY_PROMPT_H__
#define __TINY_PROMPT_H__

#ifdef ardunio
typedef tinyP_serial_t Serial;
#else // default linux
typedef int tinyP_serial_t ;
#endif

void tiny_prompt_init(tinyP_serial_t serial);

void tiny_prompt_execute(const unsigned char* serial_char_data);

#endif // __TINY_PROMPT_H__
