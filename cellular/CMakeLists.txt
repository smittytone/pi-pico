cmake_minimum_required(VERSION 3.13)
include(pico_sdk_import.cmake)
project(cellular VERSION 1.0.0)
add_executable(cellular
               cellular.cpp
               modem.cpp)

pico_sdk_init()

pico_enable_stdio_usb(cellular 1)
pico_enable_stdio_uart(cellular 1)
pico_add_extra_outputs(cellular)

target_link_libraries(cellular
                      pico_stdlib
                      hardware_gpio
                      hardware_i2c
                      hardware_adc)
