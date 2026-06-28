#!/bin/bash

# Enable debug sex if you need
# set -ex # sex (00,)

script_path=$(dirname -- "$( readlink -f -- "$0"; )")
os_name=$(uname -s)
compiler="gcc"
cc_flags="-std=c89"
cc_warnings="-Wall -Werror"

mkdir -p bin
rm -rf $script_path/bin/*

function compile_expression_x86_64_linux() {
    source="$script_path/expression.c"
    result="$script_path/bin/expression"

    $compiler $cc_flags -o $result $source
    $result
}

compile_expression_x86_64_linux
