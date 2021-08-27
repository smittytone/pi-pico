cmake_minimum_required(VERSION 3.14)
include(pico_sdk_import.cmake)
project(cellular VERSION 1.0.1)

add_definitions(-DDEBUG)

add_executable(cellular
               cellular.cpp
               modem.cpp
               mcp9808.cpp
               ht16k33.cpp
               utils.cpp
               i2c_utils.cpp
               base64.cpp)

pico_sdk_init()

pico_enable_stdio_usb(cellular 1)
pico_enable_stdio_uart(cellular 0)
pico_add_extra_outputs(cellular)

target_link_libraries(cellular
                      pico_stdlib
                      hardware_gpio
                      hardware_i2c
                      hardware_adc)
