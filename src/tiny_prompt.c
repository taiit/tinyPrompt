/**
 * @file tiny_prompt.c
 * @date 31 July 2021
 * @authors vohuutai27@gmail.com
 *
 * @brief implement tiny prompt
 */

#include <stdint.h>
#include <stdbool.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <termios.h> // pico no this
#include <unistd.h>
#include <ctype.h>
#include "tiny_prompt.h"
#include "ascii_code.h"
#include "debug.h"

#define CMD_CHARACTER_MAX   32
#define CMD_HISTORY_MAX     8

enum {
    NONE_KEY_STATE = 0,
    ESC_KEY_STATE,
    END_KEY_STATE,
    NIGA_KEY_STATE,
    DEL_KEY_STATE,
    HOME_KEY_STATE,
};

enum {
    UP = 0,
    DOWN,
    LEFT,
    RIGHT
};
#if defined(ardunio)
static const uint8_t STR_TINY_PROMPT[] = "Tiny $ ";
#elif defined(Linux)
static const uint8_t STR_TINY_PROMPT[] = "Tiny $ ";
#elif defined(PI_PICO)
static const uint8_t STR_TINY_PROMPT[] = "\nTiny $ ";
#else
#error "Unknown board"
#endif

static const uint8_t STR_TINY_PROMPT_WELCOME[] = "\n\nWelcome tiny prompt!\n";

uint8_t cmd_history[CMD_HISTORY_MAX][CMD_CHARACTER_MAX];
uint8_t cmd_buf[CMD_CHARACTER_MAX];
uint8_t key_state = NONE_KEY_STATE;

int cmd_char_cnt = 0, cmd_history_idx = 1, cmd_history_query_idx = 1, cmd_cursor_index = 0;

tinyP_serial_t tp_serial;

/**
 * Private function, send a string via serial with string length.
 */
void send_nstr(const uint8_t* str, int str_len) {
#if defined(ardunio)
#elif defined(Linux)
    size_t wlen = write(tp_serial, str, str_len); // can convert it to fwrite
    if( wlen != str_len ) {
        debug_print("Error from write: %d, %d\n", (int)wlen, errno);
    }
    tcdrain(tp_serial); /* delay for output */
#elif defined(PI_PICO)
    for(int i = 0; i < str_len; i++) {
        putchar(str[i]);
    }
#else
#error "Unknown board"
#endif
}

/**
 * Private function, send a string via serial.
 */
void send_str(const uint8_t* str) {
    size_t str_len = strlen((const char*)str);
    send_nstr(str, str_len);
}



extern function_map_t function_map [];
size_t total_func_num;

int call_function(const char *name) {
    for (size_t i = 0; i < total_func_num; i++) {
        if( !strcmp(function_map[i].name, name) && function_map[i].func ) {
            send_str("\n");
            function_map[i].func();
            return 1;
        }
    }
    debug_print("%s not found \n", name);
    send_str("\n"); send_str(name); send_str(" not found");
    return 0;
}


/**
 * Private function, insert @data into @buf at @pos and update @buf_len
 * @return true if insert data success
 */
bool insert_data(uint8_t buf[], size_t *buf_len, size_t pos, uint8_t data) {
    if (pos >= 0 && pos <= *buf_len) {
        (*buf_len)++;
        for(int i = *buf_len; i > pos; i--) {
            buf[i] =  buf[i - 1];
        }
        buf[pos] = data;
        return true;
    } else {
        debug_print("Error, incorrect insert_pos: %d, buf_len: %d\n", (int)pos, (int)(*buf_len));
        return false;
    }
}

/**
 * Private function, remove data in cmd_buf and clean history_0 if any
 */
void remove_data(int remove_count, bool clean_history_0) {
    while(remove_count-- > 0) {
        if( cmd_cursor_index > 0 ) {
            cmd_char_cnt--;
            cmd_cursor_index--;
            for (int idx = cmd_cursor_index; idx < cmd_char_cnt; idx++) {
                cmd_buf[idx] = cmd_buf[idx + 1];
            }
            cmd_buf[cmd_char_cnt] = '\0';
            send_str((const uint8_t*) "\x1b[1D"); // Moves the cursor backward
            send_str((const uint8_t*) (&cmd_buf[cmd_cursor_index]));
            send_str((const uint8_t*) " ");
            for (int idx = cmd_char_cnt; idx >= cmd_cursor_index; idx--) {
                send_str((const uint8_t*) "\x1b[1D"); // Moves the cursor backward
            }
            if(clean_history_0) {
                memcpy(cmd_history[0], cmd_buf, sizeof(cmd_buf));
            }
        }
    }
}

void delete_data(int del_cnt) {
    while(del_cnt-- > 0) {
        if (cmd_cursor_index >= 0 && cmd_cursor_index < cmd_char_cnt) {
            for(int idx = cmd_cursor_index; idx < cmd_char_cnt; idx++) {
                cmd_buf[idx] = cmd_buf[idx+1];
            }
            cmd_char_cnt--;
            cmd_buf[cmd_char_cnt] = '\0';
            if (cmd_cursor_index > 0) {
                send_str((const uint8_t*) "\x1b[1D"); // Moves the cursor backward
                send_str((const uint8_t*) (&cmd_buf[cmd_cursor_index - 1]));
                send_str((const uint8_t*) " ");
            } else {
                send_str((const uint8_t*) (&cmd_buf[cmd_cursor_index]));
                send_str((const uint8_t*) " ");
            }
            for(int idx = cmd_char_cnt; idx >= cmd_cursor_index; idx--) {
                send_str((const uint8_t*) "\x1b[1D"); // Moves the cursor backward
            }
        }
    }
}

bool is_in_history_list(const uint8_t* data) {
    for(size_t i = 1; i < CMD_HISTORY_MAX; i++) {
        if(strcmp((const char*)cmd_history[i], (const char*)data) == 0) {
            return true;
        }
    }
    return false;
}

static int find_history_slot() {
    for(int i = (CMD_HISTORY_MAX - 1); i > 0; i-- ) {
        if(strlen((const char*)cmd_history[i]) > 0) {
            return i;
        }
    }
    return 0;
}

void handling_up_down_history(int directon) {
    debug_print("\thistory_idx: %d, history_query_idx: %d\n",
            cmd_history_idx, cmd_history_query_idx);
    size_t history_slot = find_history_slot();
    if(cmd_history_query_idx == -1) {
        if(directon == UP) {
            cmd_history_query_idx = cmd_history_idx;
        } else {
            cmd_history_query_idx = (history_slot == (CMD_HISTORY_MAX - 1)) ? cmd_history_idx + 1 : 1;
        }
    } else {
        if (directon == UP) {
           cmd_history_query_idx = (cmd_history_query_idx - 1 >= 0) ?
                   cmd_history_query_idx - 1 : history_slot;
        } else {
           cmd_history_query_idx = (cmd_history_query_idx + 1 <= history_slot) ?
                   cmd_history_query_idx + 1 : 0;
        }
    }

        delete_data(cmd_char_cnt - cmd_cursor_index);
        remove_data(cmd_cursor_index, 0);

        cmd_cursor_index = cmd_char_cnt = strlen((const char*)cmd_history[cmd_history_query_idx]);
        memcpy(cmd_buf, cmd_history[cmd_history_query_idx],
                (cmd_char_cnt + 1) * sizeof(uint8_t)); // +1: include '\0' character

        send_str((const uint8_t*)cmd_buf);
}

void save_visible_character(const uint8_t chr) {
    //send_nstr(character, 1); // send back received data

    if ( isprint(chr) ) { // isprint from ctype.h
        send_nstr(&chr, 1);
        if( insert_data(cmd_buf, (size_t*)(&cmd_char_cnt), (size_t)cmd_cursor_index, chr) ) {
            memcpy(cmd_history[0], cmd_buf, (strlen((const char*) cmd_buf) + 1) * sizeof(uint8_t));
            cmd_cursor_index++;
            if( cmd_cursor_index < cmd_char_cnt ) {
                send_str((const uint8_t*) (&cmd_buf[cmd_cursor_index]));
            }
            for (int idx = cmd_cursor_index; idx < cmd_char_cnt; idx++) {
                send_str((const uint8_t*) "\x1b[1D"); // Moves the cursor backward
            }
        }
    }
}

void handle_enter_key() {
    if (cmd_char_cnt > 0) {
        debug_print("\n ENTER KEY: cmd_buf: %s \n", cmd_buf);
        if(!is_in_history_list(cmd_buf)) {
            if(strlen((const char *)cmd_history[cmd_history_idx]) > 0) {
                cmd_history_idx = cmd_history_idx + 1 < CMD_HISTORY_MAX ? cmd_history_idx + 1 : 1;
            }
            memcpy(cmd_history[cmd_history_idx], cmd_buf, sizeof(cmd_buf));
        }
        memset(cmd_history[0], 0, strlen((const char*)cmd_history[0]) * sizeof(unsigned char));
        // call function
        call_function((const char*)cmd_buf);
        // reset cmd_bufs
        cmd_char_cnt = cmd_cursor_index = 0;
        memset(cmd_buf, 0, sizeof(cmd_buf));
    }
    cmd_history_query_idx = -1;
    send_str(STR_TINY_PROMPT);
}


void tiny_prompt_init(tinyP_serial_t serial, size_t func_num) {
    tp_serial = serial;
    memset(cmd_buf, 0, sizeof(cmd_buf));
    // clean-up history
    for (int i = 0; i < CMD_HISTORY_MAX; i++) {
        memset(cmd_history[i], 0, sizeof(cmd_history[i]));
    }
    send_str(STR_TINY_PROMPT_WELCOME);
    send_str(STR_TINY_PROMPT);
    total_func_num = func_num;
}

/**
 *
 * @param serial_char_data
 */
void tiny_prompt_execute(const uint8_t serial_char_data) {
    char is_debug = 1;

    switch(serial_char_data) {
        case ASCII_CODE_BS:     // 0x08 (backspace).
            remove_data(1, 1);
            break;
        case ASCII_CODE_CR: // 0x0D (enter).
            send_nstr(&serial_char_data, 1);
            handle_enter_key();
            break;
        case ASCII_CODE_ESC: // 0x1b
            /*
             * 0x1b 0x5b 0x31 0x7e      = HOME
             * 0x1b 0x5b 0x33 0x7e      = DEL
             * 0x1b 0x5b 0x41           = UP
             * 0x1b 0x5b 0x42           = DOWN
             * 0x1b 0x5b 0x43           = RIGHT
             * 0x1b 0x5b 0x44           = LEFT
             */
            debug_print("*Specical esc key\n");
            key_state = ESC_KEY_STATE;
            is_debug = 0;
            break;
        case 0x5b:
            if (ESC_KEY_STATE == key_state) {
                key_state = NIGA_KEY_STATE;
            } else {
                save_visible_character(serial_char_data);
            }
            is_debug = 0;
            break;
        case 0x31: // check HOME key
        case 0x33: // check DEL key
        case 0x41: // up
        case 0x42: // down
        case 0x43: // right
        case 0x44: // left
            if (NIGA_KEY_STATE == key_state) {
                if( serial_char_data == 0x41 ) {
                    handling_up_down_history(UP);
                    key_state = NONE_KEY_STATE;
                } else if( serial_char_data == 0x42 ) {
                    handling_up_down_history(DOWN);
                    key_state = NONE_KEY_STATE;
                } else if( serial_char_data == 0x43 ) {
                    debug_print("RIGHT\n");
                    if (cmd_cursor_index < cmd_char_cnt) {
                        send_str((const uint8_t*) "\x1b[1C"); // cursor forward
                        cmd_cursor_index++;
                    }
                    key_state = NONE_KEY_STATE;
                } else if( serial_char_data == 0x44 ) {
                    debug_print("LEFT\n");
                    if(cmd_cursor_index > 0) {
                        send_str((const uint8_t*) "\x1b[1D"); // cursor backward
                        cmd_cursor_index--;
                    }
                    key_state = NONE_KEY_STATE;
                } else if ( serial_char_data == 0x33 ) { // check DEL
                   key_state = DEL_KEY_STATE;
                } else if ( serial_char_data == 0x31 ) { // check HOME
                   key_state = HOME_KEY_STATE;
                }
            } else {
                save_visible_character(serial_char_data);
            }
            break;
        case 0x4f:
            if (ESC_KEY_STATE == key_state) {
                key_state = END_KEY_STATE;
            } else {
                save_visible_character(serial_char_data);
            }
            break;
        case 0x46:
            if (END_KEY_STATE == key_state) {
                debug_print("END\n");
                for (; cmd_cursor_index < cmd_char_cnt; ++cmd_cursor_index) {
                    send_str((const uint8_t*) "\x1b[1C"); // Moves the cursor forward
                }
                key_state = NONE_KEY_STATE;
            } else {
                save_visible_character(serial_char_data);
            }
            break;
        case 0x7e:
            if(DEL_KEY_STATE == key_state) {
                debug_print("DEL\n");
                delete_data(1);
                key_state = NONE_KEY_STATE;
            } else if (HOME_KEY_STATE == key_state) {
                debug_print("HOME\n");
                for(; cmd_cursor_index > 0; --cmd_cursor_index) {
                    send_str((const uint8_t*) "\x1b[1D");
                }
                key_state = NONE_KEY_STATE;
            } else {
                save_visible_character(serial_char_data);
            }
            break;
        default:
            save_visible_character(serial_char_data);
            break;
    }// end switch

// Debug session
    if(is_debug) {
        debug_print("---------------------------------------------------------------------");
        debug_print("Input character: [ 0x%x - %c ]",
                (unsigned int)serial_char_data, isprint(serial_char_data) ? serial_char_data : (char*)" ");

        debug_print("cursor_index: %d, char_cnt: %d, history_idx: %d, history_query_idx: %d",
                cmd_cursor_index, cmd_char_cnt, cmd_history_idx, cmd_history_query_idx);
        if (cmd_char_cnt > 0) {
            for(int idx = 0; idx < cmd_char_cnt; idx++) {
                if(idx == cmd_cursor_index) {
                    debug_print("\t\x1b[4m%c\033[0m", cmd_buf[idx]); // print under line charactor
                } else {
                    debug_print("\t%c", cmd_buf[idx]);
                }
            }
            if (cmd_cursor_index == cmd_char_cnt) {
                debug_print("\t_");
            }
        }
        debug_print("History");
        for(int i = 0; i < CMD_HISTORY_MAX; i++) {
            debug_print("\thistory[%d]: %s", i, cmd_history[i]);
        }
        debug_print("---------------------------------------------------------------------\n");
    }
}
