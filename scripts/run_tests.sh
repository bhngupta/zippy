#!/bin/bash

rm -rf build

mkdir build

cd build

cmake ..

make

cd build

make run_tests

cd ..

rm -rf build