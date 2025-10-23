#! /usr/bin/bash

# compile into created folder /build 
cmake -S . -B build

# create executible / library from /build folder
cmake --build build 

