#!/bin/bash
clear &&
    rm -rf ./build &&
    mkdir ./build &&
    cd ./build &&
    cmake -S .. -B . &&
    sudo make install &&
    echo "tfetch was successfully installed"
