#!/bin/bash
clear &&
    rm -rf ./build &&
    mkdir ./build &&
    cd ./build &&
    cmake -S .. -B . &&
    make &&
    ./tfetch
