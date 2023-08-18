#!/bin/bash


if [[ $# -lt 2 ]]; then
    echo "not enough arguments $#"
    exit 1;
elif [[ ! -d $1 ]]; then
    echo "$1 is not a folder"
    exit 1;
else
    num_of_files=$(find $1 -type f | wc -l)
    num_of_lines=$(grep -r $2 $1 | wc -l)
    echo "The number of files are $num_of_files and the number of matching lines are $num_of_lines"
fi
