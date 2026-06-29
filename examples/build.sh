#!/bin/bash

# Enable debug sex if you need
# set -ex # sex (00,)

script_path=$(dirname -- "$( readlink -f -- "$0"; )")
compiler="gcc"
cc_flags="-std=c89"
cc_warnings="-Wall -Werror -g3"

mkdir -p bin
rm -rf $script_path/bin/*

function compile_expression() {
    source="$script_path/expression.c"
    result="$script_path/bin/expression"

    $compiler $cc_flags -o $result $source
    $result
}

function compile_if_statement() {
    source="$script_path/if_statement.c"
    result="$script_path/bin/if_statement"

    $compiler $cc_flags -o $result $source
    $result
}

function compile_json() {
    source="$script_path/json.c"
    result="$script_path/bin/json"

    $compiler $cc_flags -o $result $source
    $result
}

echo "Running expression parsing example..."
compile_expression
echo "Running if statement parsing example..."
compile_if_statement
echo "Running json parsing example..."
compile_json
