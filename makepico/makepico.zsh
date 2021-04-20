#!/bin/zsh

#
# makepico.zsh
#
# Create a baseline Raspberry Pi Pico C-language project
#
# @author    Tony Smith
# @copyright 2021, Tony Smith
# @version   2.0.0
# @license   MIT
#


# FROM 1.0.2
# Check that PICO_SDK_PATH is defined
# and the SDK is installed
if [[ -z "${PICO_SDK_PATH}" ]]; then
    echo "Error — environment variable PICO_SDK_PATH not set"
    exit 1
else
    ls "${PICO_SDK_PATH}" >/dev/null 2>&1
    if [[ $? -ne 0 ]]; then
        echo "Error — Pico SDK not installed at ${PICO_SDK_PATH}"
        exit 1
    fi
fi


show_help() {
    echo -e "\nMake a Pi Pico Project\n"
    echo -e "Usage:\n  makepico [path/name] [-d] [-h]\n"
    echo    "Options:"
    echo    "  -c / --cpp     Set up the project for C++. Default: false"
    echo    "  -d / --debug   Set up the project for SWD. Default: false"
    echo    "  -n / --name    Your name for the comments. Default: <YOU>"
    echo    "  -h / --help    This help screen"
    echo
}

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

    # Create initial C files
    make_source_files "${1}"

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

    # FROM 1.0.2
    make_vscode "${1}"

    # And done...
    project_name=${1:t}
    echo "Project ${project_name} created at ${project_path}"
}

make_source_files() {
    # Output lines to the file
    # Args: 1 -- project path

    echo "Creating project files..."
    project_name=${1:t}
    source_file=${1:t:l}

    # FROM 1.3.0
    file_ext="c"
    if [[ $do_cpp -eq 1 ]]; then
        file_ext="cpp"
    fi

    write_header "$project_name" "${1}/${source_file}.${file_ext}"
    write_header "$project_name" "${1}/${source_file}.h"

    # FROM 1.3.0
    # Break into sections for C/C++ usage

    {
        echo "#ifndef _${project_name:u}_HEADER_"
        echo "#define _${project_name:u}_HEADER_"
        echo
    } >> "${1}/${source_file}.h"

    # C++ standard libraries or C standard libraries
    if [[ $do_cpp -eq 1 ]]; then
        {
            echo '#ifdef __cplusplus'
            echo '    extern "C" {'
            echo '#endif'
            echo
            echo '#include <iostream>'
            echo '#include <cstdlib>'
            echo '#include <string> using std::string'
            echo '#include <vector> using std::vector'
            echo
        } >> "${1}/${source_file}.h"
    else
                {
            echo '#include <stdbool.h>'
            echo '#include <stdio.h>'
            echo '#include <stdlib.h>'
            echo '#include <string.h>'
            echo '#include <time.h>'
        } >> "${1}/${source_file}.h"
    fi

    # Standard Pico libraries
    {
        echo '#include "pico/stdlib.h"'
        echo '#include "pico/binary_info.h"'
        echo '#include "hardware/gpio.h"'
        echo '#include "hardware/i2c.h"'
        echo '#include "hardware/adc.h"'
        echo
    } >> "${1}/${source_file}.h"

    if [[ $do_cpp -eq 1 ]]; then
        {
            echo '#ifdef __cplusplus'
            echo '}'
            echo '#endif'
            echo
        } >> "${1}/${source_file}.h"
    fi

    {
        echo "#endif // _${project_name:u}_HEADER_"
    } >> "${1}/${source_file}.h"
}

write_header() {
    # Write header info
    {
        echo "/*"
        echo " * ${1} for Raspberry Pi Pico"
        echo " *"
        echo " * @version     1.0.0"
        echo " * @author      ${users_name}"
        echo " * @copyright   $(date +'%Y')"
        echo " * @licence     MIT"
        echo " *"
        echo " */"
    } > "$2"
}

make_cmake_file() {
    # Output lines to the file, interpolating the project name
    # Args: 1 -- project path

    echo "Creating CMakeLists.txt..."
    project_name=${1:t}
    source_file=${project_name:l}

    # FROM 1.3.0
    file_ext="c"
    if [[ $do_cpp -eq 1 ]]; then
        file_ext="cpp"
    fi

    {
        echo 'cmake_minimum_required(VERSION 3.13)'
        echo 'include(pico_sdk_import.cmake)'
        echo "project(${project_name} VERSION 1.0.0)"
        echo "add_executable(${project_name}"
        echo "               ${source_file}.${file_ext})"
        echo
        echo "pico_sdk_init()"
        echo
        echo "pico_enable_stdio_usb(${project_name} 1)"
        echo "pico_enable_stdio_uart(${project_name} 1)"
        echo "pico_add_extra_outputs(${project_name})"
        echo
        echo "target_link_libraries(${project_name}"
        echo '                      pico_stdlib'
        echo '                      hardware_gpio'
        echo '                      hardware_i2c'
        echo '                      hardware_adc)'
    } >> "${1}/CMakeLists.txt"
}

make_vscode() {
    # FROM 1.0.2
    # Make the vscode settings
    echo "Configuring VSCode..."
    mkdir "${1}/.vscode"
    {
        echo '{'
        echo '    "cmake.environment": {'
        echo "        \"PICO_SDK_PATH\": \"${PICO_SDK_PATH}\""
        echo '    },'
        echo '    "C_Cpp.default.configurationProvider": "ms-vscode.cmake-tools\"'
        echo '}'
    } >> "${1}/.vscode/settings.json"

    # Debug flag set? write out an SWD launch config
    if [[ $do_swd -eq 1 ]]; then
        {
            echo '{'
            echo '    "version": "0.2.0",'
            echo '    "configurations": ['
            echo '        {   "name": "Pico Debug",'
            echo '            "device": "RP2040",'
            echo '            "gdbPath": "arm-none-eabi-gdb",'
            echo '            "cwd": "\${workspaceRoot}",'
            echo '            "executable": "\${command:cmake.launchTargetPath}",'
            echo '            "request": "launch",'
            echo '            "type": "cortex-debug",'
            echo '            "servertype": "openocd",'
            echo '            "configFiles": ['
            echo '                "/interface/picoprobe.cfg",'
            echo '                "/target/rp2040.cfg"'
            echo '            ],'
            echo '            "svdFile": "\${env:PICO_SDK_PATH}/src/rp2040/hardware_regs/rp2040.svd",'
            echo '            "runToMain": true,'
            echo '            "postRestartCommands": ['
            echo '                "break main",'
            echo '                "continue"'
            echo '            ]'
            echo '        }'
            echo '    ]'
            echo '}'
        } >> "${1}/.vscode/launch.json"
    fi
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
projects=()
do_swd=0
do_cpp=0
users_name="<YOU>"
next_is_arg=0
last_arg=""

for arg in "$@"; do
    uarg=${arg:u}
    if [[ $next_is_arg -gt 0 ]]; then
        # The argument should be a value (previous argument was an option)
        if [[ ${arg:0:1} = "-" ]]; then
            # Next value is an option: ie. missing value
            echo "Error -- Missing value for ${last_arg}"
            exit 1
        fi

        # Set the appropriate internal value
        case "$next_is_arg" in
            1) users_name=$arg ;;
            *) echo "Error -- Unknown argument" exit 1 ;;
        esac

        # Reset
        next_is_arg=0
    else
        if [[ "$uarg" == "-N" || "$uarg" == "--NAME" ]]; then
            next_is_arg=1
        elif [[ "$uarg" == "-C" || "$uarg" == "--CPP" ]]; then
            do_cpp=1
        elif [[ "$uarg" == "-D" || "$uarg" == "--DEBUG" ]]; then
            do_swd=1
        elif [[ "$uarg" == "-H" || "$uarg" == "--HELP" ]]; then
            show_help
            exit 0
        else
            projects+=("$arg")
        fi

        last_arg=$uarg
    fi
done

if [[ ${#projects[@]} -gt 0 ]]; then
    for project in "${projects[@]}"; do
        make_project "$project"
    done
else
    show_help
    exit 0
fi
