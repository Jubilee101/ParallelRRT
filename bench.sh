#!/bin/bash

OUT=summary.csv

ITERATION=5

TIMEFORMAT='%3R'

BIN=./build/rrt

ALG="1 2"

CPUS="1 2 4 8"

echo CPUs "," ALG ","ITERATION"," TIME"," NODE SIZE >$OUT
echo Phase "," CPUs "," ALG ","ITERATION"," TIME >detect_obstacles.csv
echo Phase "," CPUs "," ALG ","ITERATION"," TIME >find_nearest.csv
echo Phase "," CPUs "," ALG ","ITERATION"," TIME >inflate_obstacles.csv
echo Phase "," CPUs "," ALG ","ITERATION"," TIME >total.csv
for c in $CPUS; do
	export OMP_NUM_THREADS=$c
	export OMP_CANCELLATION=true
	for a in $ALG; do
		for r in $(seq 1 $ITERATION); do
			./$BIN $a $r $c
		done
	done
done
