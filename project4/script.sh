#!/bin/bash

# number of threads
for size in 1000 5000 10000 20000 40000 60000 80000 100000 150000 200000 250000 300000 350000 400000
do
    echo ARRAYSIZE = $size

    g++  -DARRAYSIZE=$size -o simd.p4.cpp  simd.p4.cpp simd.p4.o  -lm  -fopenmp
    ./simd.p4
done

