#!/bin/bash

OUT=summary.csv

ITERATION=5

TIMEFORMAT='%3R'

BIN=./build/rrt

ALG="1 2"

CPUS="1 2 4"

echo CPUs "," ALG ","ITERATION"," TIME > $OUT

for c in $CPUS
do
	export OMP_NUM_THREADS=$c
	for a in $ALG
	do
		for r in `seq 1 $ITERATION`
		do
			./$BIN  $a $r $c
		done
	done
done
