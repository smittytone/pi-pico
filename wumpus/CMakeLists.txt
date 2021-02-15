cmake_minimum_required(VERSION 3.12)
include(pico_sdk_import.cmake)
project(Wumpus)
add_executable(Wumpus wumpus.c)
pico_sdk_init()
pico_enable_stdio_usb(Wumpus 1)
pico_enable_stdio_uart(Wumpus 0)
pico_add_extra_outputs(Wumpus)
target_link_libraries(
    Wumpus
    pico_stdlib
    hardware_i2c
    hardware_pwm
    hardware_gpio
    hardware_adc)

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DTSDEBUG")