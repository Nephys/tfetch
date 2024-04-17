#!/bin/bash
clear &&
    rm -rf ./build &&
    mkdir ./build &&
    cd ./build &&
    cmake -S .. -B . -DCMAKE_BUILD_TYPE=Debug &&
    make &&
    valgrind --track-origins=yes --leak-check=full --show-leak-kinds=all --fair-sched=yes -s ./tfetch
