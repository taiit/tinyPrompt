/**
 * @file main.c
 * @date 31 July 2021
 * @authors vohuutai27@gmail.com
 *
 * @brief test program, which used tiny prompt
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/bootrom.h"
#include "pico/stdlib.h"

#include "../src/tiny_prompt.h"

void help();
void reboot();
void led_on();
void led_off();

const uint LED_PIN = 25;

extern size_t total_func_num;

function_map_t function_map [] = {
    { "help",           help        },
    { "reboot",         reboot      },
    { "led_on",         led_on      },
    { "led_off",        led_off     }
};


/**
 * @brief main program entry
 * @return never return as normal
 */
int main() {

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    // Initial I/O
    stdio_init_all();

    char input_char;
    size_t func_num = (sizeof(function_map) / sizeof(function_map[0]));
    tiny_prompt_init(0, func_num);

    while (true) {
        //Get user input & execute tiny prompt
        input_char = getchar();
        tiny_prompt_execute(input_char);
    }
    return 0;
}


void help() {
    printf("%s() Supported function: ", __func__);
    for (int i = 0; i < total_func_num; i++) {
        printf("    %s", function_map[i].name);
    }
}

void reboot() {
    printf("Rebooting...\n");
    // pico reset function
    reset_usb_boot(0, 0);
}

void led_on() {
    printf("Led on\n");
    gpio_put(LED_PIN, 1);
}

void led_off() {
    printf("Led off\n");
    gpio_put(LED_PIN, 0);
}

