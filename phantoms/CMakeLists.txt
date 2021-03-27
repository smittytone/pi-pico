cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(phantoms VERSION 1.0.1)

add_executable(phantoms
    main.c
    gfx.c
    map.c
    phantoms.c
    ssd1306.c
    tinymt32.c
)

pico_sdk_init()

pico_enable_stdio_usb(phantoms 1)

pico_enable_stdio_uart(phantoms 1)

pico_add_extra_outputs(phantoms)

target_link_libraries(
    phantoms
    pico_stdlib
    hardware_gpio
    hardware_i2c
    hardware_adc
)
