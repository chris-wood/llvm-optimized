#!/bin/bash
for var in "$@"
do
    echo "Running with: " $var
    ./clang -03 -emit-llvm $var.c -c -o $var.bc
    ./opt -insert-edge-profiling $var.bc -o $var.profile.bc
	./llc $var.profile.bc -o $var.profile.s
	./clang -o $var.profile $var.profile.s ../lib/libprofile_rt.so

	# Gather the profile data and output, for peace of mind
	./$var.profile > $var.profile.out
	./llvm-prof $var.profile.bc

	### OLD SCRIPT TO RUN ON THE INSTRUMENTED VERSION!

	# # Now do the block placement and gather the results...
	# ./opt -profile-loader -block-placement $var.profile.bc
	# ./llc $var.profile.bc -o $var.profile_mod.s
	# ./clang -o $var.profile_mod $var.profile_mod.s ../lib/libprofile_rt.so
	# ./$var.profile_mod

	# # Now run the program with the time script and save the output
	# COUNTER=0
	# while [  $COUNTER -lt 5 ]; do
	# 	echo Running iteration $COUNTER and storing the time
	# 	perl time.pl $var.profile     > $var.profile.out.$COUNTER
	# 	perl time.pl $var.profile_mod > $var.profile.out.$COUNTER
	# 	let COUNTER=COUNTER+1
	# done

	# Now do the block placement and gather the results...
	./opt -profile-loader -block-placement $var.bc
	./llc $var.bc -o $var.mod.s
	./clang -o $var.mod $var.mod.s ../lib/libprofile_rt.so
	./$var.mod > $var.mod.out
	./clang -o $var $var.c

	# Now run the program with the time script and save the output
	COUNTER=0
	while [  $COUNTER -lt 5 ]; do
		echo Running iteration $COUNTER and storing the time
		perl time.pl $var      > $var.out.$COUNTER
		perl time.pl $var.mod  > $var.mod.out.$COUNTER
		let COUNTER=COUNTER+1
	done
done


