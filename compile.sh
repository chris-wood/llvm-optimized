#!/bin/bash
for var in "$@"
do
    echo "Running with: " $var
    ./clang -03 -emit-llvm $var.c -c -o $var.bc
    ./opt -insert-edge-profiling $var.bc -o $var.profile.bc
	./llc $var.profile.bc -o $var.profile.s
	./clang -o $var.profile $var.profile.s ../lib/libprofile_rt.so
	./$var.profile
	./llvm-prof $var.profile.bc
	./opt -profile-loader -block-placement $var.profile.bc
done


