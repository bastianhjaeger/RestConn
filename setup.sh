#!/bin/bash

sudo apt install libcurl4-openssl-dev libgtest-dev

mkdir build 
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
