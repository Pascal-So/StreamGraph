#!/bin/bash

if [ -z "$1" ]; then
    echo "Input file not provided." >&2
    echo "Usage:" $0 "<input file location>" >&2
    exit 1
fi

cat "$1" |
    sed '/^\s*#.*$/d' |
    sed '/^\s*$/d' |
    sed 's/^\s*//' |
    sed -E 's/function ([a-zA-Z0-9_]+) \{/\1 () {/'
