#!/bin/bash

rm -rf build

mkdir build

cd build

cmake ..

make

./zippy -f ../logger.log