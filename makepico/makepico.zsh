#!/bin/zsh

#
# makepico.zsh
#
# Create a baseline Raspberry Pi Pico C-language project
#
# @author    Tony Smith
# @copyright 2021, Tony Smith
# @version   1.0.0
# @license   MIT
#


make_project() {
    # Create and copy the project files
    # Args: 1 -- the project path

    # Check the path and, if necessary
    # create intermediate directories
    # NOTE The check will exit on error
    check_path "${1}"

    # Get project name in lower case
    project_name=${1:t:l}
    project_path=${1}

    # Create initial C file
    make_source_file "${1}"

    # Copy over the .make file from the SDK
    file="pico_sdk_import.cmake"
    if cp "${PICO_SDK_PATH}/external/${file}" "${project_path}/${file}"; then
        # NOP
    else
        echo "Error - could not copy the ${file} file"
        exit 1
    fi

    # Make the CMakeLists.txt file for this project
    make_cmake_file "${1}"

    # And done...
    project_name=${1:t}
    echo "Project ${project_name} created at ${project_path}"
}

make_source_file() {
    # Output lines to the file
    # Args: 1 -- project path

    echo "Creating project files..."
    project_name=${1:t}
    source_file=${1:t:l}
    {
        echo -e "/*\n *    Project ${project_name} created by makepico\n */\n\n"
        echo '#include <stdio.h>'
        echo '#include "pico/stdlib.h"'
        echo '#include "pico/binary_info.h"'
        echo '#include "hardware/gpio.h"'
        echo -e "\n\nint main() {\n    return 0;\n}\n"
    } >> "${1}/${source_file}.c"
}

make_cmake_file() {
    # Output lines to the file, interpolating the project name
    # Args: 1 -- project path

    echo "Creating CMakeLists.txt..."
    project_name=${1:t}
    source_file=${project_name:l}
    {
        echo "cmake_minimum_required(VERSION 3.12)"
        echo "include(pico_sdk_import.cmake)"
        echo "project(${project_name})"
        echo "add_executable(${project_name} ${source_file}.c)"
        echo "pico_sdk_init()"
        echo "pico_enable_stdio_usb(${project_name} 1)"
        echo "pico_enable_stdio_uart(${project_name} 1)"
        echo "pico_add_extra_outputs(${project_name})"
        echo "target_link_libraries(${project_name} pico_stdlib)"
    } >> "${1}/CMakeLists.txt"
}

check_path() {
    # Check the path is valid
    # Args: 1 -- the project path as specified by the user
    project_name={$1:t}
    if [[ ! -d "${1}" ]]; then
        if mkdir -p "${1}" >> /dev/null; then
            echo "Creating project directory..."
        else
            echo "Error — could not create path for project ${project_name}"
            exit 1
        fi
    fi
}

# Runtime start
# Get the arguments, which should be project path(s)
for arg in "$@"; do
    make_project $arg
done