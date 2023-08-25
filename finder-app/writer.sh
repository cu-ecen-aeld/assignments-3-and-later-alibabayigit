#!/bin/sh

if [[ $# -lt 2 ]]; then
    exit 1;
fi
mkdir -p $(dirname $1)
if touch $1; then
    echo "$2" > $1
else
    exit 1
fi
