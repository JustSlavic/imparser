#!/bin/bash

# Enable debug sex if you need
# set -ex # sex (00,)

script_path=$(dirname -- "$( readlink -f -- "$0"; )")
compiler="gcc"
cc_flags="-std=c89 -g"
cc_warnings="-Wall -Werror"

mkdir -p bin
rm -rf $script_path/bin/*

function compile_example() {
    source=$1
    result=$2

    $compiler $cc_flags -o $result $source
    $result
}

echo "Running expression parsing example..."
compile_example "$script_path/expression.c" "$script_path/bin/expression"
echo "Running if statement parsing example..."
compile_example "$script_path/if_statement.c" "$script_path/bin/if_statement"
echo "Running json parsing example..."
compile_example "$script_path/json.c" "$script_path/bin/json"
echo "Running S-expression example..."
compile_example "$script_path/s_expression.c" "$script_path/bin/sexp"
