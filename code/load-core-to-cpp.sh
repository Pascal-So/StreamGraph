#!/bin/bash

if [ -z "$1" ]; then
    echo "Input file not provided." >&2
    echo "Usage:" $0 "<input file location>" >&2
    exit 1
fi

echo "#include<bits/stdc++.h>"
echo "std::string sg_core = "

cat "$1" |
    sed 's/\\/\\\\/g' |
    sed 's/"/\\"/g' |
    sed 's/^/"/' |
    sed 's/$/\\n"/'

echo ";"
