#!/bin/bash

filename=$1
filename_only="${filename%.*}"

# compile
clang++ -std=c++20 "$filename" -o "$filename_only" -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio

echo "compiled to: [$filename_only]"
