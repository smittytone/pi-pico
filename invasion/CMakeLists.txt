cmake_minimum_required(VERSION 3.13)
message(STATUS "Reading CMakeLists.txt")
include(pico_sdk_import.cmake)
project(planet_invasion
        VERSION 1.0.0
        HOMEPAGE_URL https://smittytone.net/pico-planet-invasion)

add_executable(planet_invasion
               entities.c
               gfx.c
               invaders.c
               invasion.c
               ssd1306.c
               tinymt32.c
               utils.c)

pico_sdk_init()
pico_enable_stdio_usb(planet_invasion 1)
pico_enable_stdio_uart(planet_invasion 1)
pico_add_extra_outputs(planet_invasion)

target_link_libraries(planet_invasion
                      pico_stdlib
                      hardware_gpio
                      hardware_i2c
                      hardware_adc)
