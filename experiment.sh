#!/bin/bash

./compile.sh is_prime 5 1024 2048 4096 8192 16384
./compile.sh block_test 5 1000 2000 3000 4000 5000
./compile.sh blocked 5 500 600 700 800 900 1000
./compile.sh factor 5 1000000 2000000 3000000 4000000 5000000
./compile.sh bubble 5 50000 100000 150000 200000 250000
./compile.sh kmeans 5 100000 200000 300000 400000 500000
./compile.sh permutations 5 6 7 8 9 10
./compile.sh mandlebrot 5 500 600 700 800 900 1000
