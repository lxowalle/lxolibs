#!/bin/bash

get_c_file()
{
    files=$(ls -RL $1)
    for filename in $files
    do
        echo xxx_SOURCES += $filename
    done
}

get_c_file ../../third_party/pjproject