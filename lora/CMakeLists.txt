cmake_minimum_required(VERSION 3.14)
include(pico_sdk_import.cmake)
project(lora
        VERSION 1.0.0
        DESCRIPTION "LoRa Demo"
        HOMEPAGE_URL "https://github.com/smittytone/pi-pico")

add_definitions(-DDEBUG)

pico_sdk_init()

add_executable(lora_sender
               base64.cpp
               led_utils.cpp
               main_sender.cpp
               reg_bits.cpp
               rfm9x.cpp
               spi_utils.cpp)

pico_enable_stdio_usb(lora_sender 1)
pico_add_extra_outputs(lora_sender)

target_link_libraries(lora_sender
                      pico_stdlib
                      hardware_gpio
                      hardware_spi)

add_executable(lora_receiver
               base64.cpp
               led_utils.cpp
               main_receiver.cpp
               reg_bits.cpp
               rfm9x.cpp
               spi_utils.cpp)

pico_enable_stdio_usb(lora_receiver 1)
pico_add_extra_outputs(lora_receiver)

target_link_libraries(lora_receiver
                      pico_stdlib
                      hardware_gpio
                      hardware_spi)