#!/bin/bash

rm -rf build

mkdir build

cd build

cmake ..

make

mkdir -p ../logs

./zippy -f ../logs/logger_$(date +%Y-%m-%d_%H-%M-%S).log