cmake_minimum_required(VERSION 3.13)
include(pico_sdk_import.cmake)
project(ski VERSION 1.0.0)
add_executable(ski
               ski.cpp
               ssd1306.cpp
               tinymt32.c
               utils.cpp)

pico_sdk_init()

pico_enable_stdio_usb(ski 1)
pico_enable_stdio_uart(ski 1)
pico_add_extra_outputs(ski)

target_link_libraries(ski
                      pico_stdlib
                      hardware_gpio
                      hardware_i2c
                      hardware_adc)
