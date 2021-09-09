cmake_minimum_required(VERSION 3.14)
include(pico_sdk_import.cmake)
project(lora
        VERSION 1.0.0
        DESCRIPTION "LoRa Demo"
        HOMEPAGE_URL "https://github.com/smittytone/pi-pico")

add_definitions(-DDEBUG)

add_executable(lora
               base64.cpp
               main.cpp
               reg_bits.cpp
               rfm9x.cpp
               spi_utils.cpp)

pico_sdk_init()
pico_enable_stdio_usb(lora 1)
pico_enable_stdio_uart(lora 0)
pico_add_extra_outputs(lora)

target_link_libraries(lora
                      pico_stdlib
                      hardware_gpio
                      hardware_spi)
