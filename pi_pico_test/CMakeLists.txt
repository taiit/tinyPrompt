cmake_minimum_required(VERSION 3.12)

set(PRJ_NAME tinyPromptPico)

# Pull in SDK (must be before project)
include(pico_sdk_import.cmake)

project(${PRJ_NAME} C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialize the SDK
pico_sdk_init()

include_directories("../src")	#This will be similar to calling gcc with -I../src option. 
# Add source code
add_executable(${PRJ_NAME}
        main.c
		../src/tiny_prompt.c
    )

# Pull in our pico_stdlib which pulls in commonly used features
target_link_libraries(${PRJ_NAME}
		pico_stdlib
	)

# enable/disable usb output,uart output
pico_enable_stdio_usb(${PRJ_NAME} 0)
pico_enable_stdio_uart(${PRJ_NAME} 1)

# create map/bin/hex file etc.
pico_add_extra_outputs(${PRJ_NAME})

