#!/bin/bash

OUT=summary.csv

ITERATION=10

TIMEFORMAT='%3R'

BIN=./build/rrt


CPUS="1 2 4 8"

MAP_SELECTION=$1

# echo CPUs "," ALG ","ITERATION"," TIME"," NODE SIZE >$OUT
# echo Phase "," CPUs "," ALG ","ITERATION"," TIME"," CALLCOUNT"," SINGLETIME >detect_obstacles.csv
# echo Phase "," CPUs "," ALG ","ITERATION"," TIME"," CALLCOUNT"," SINGLETIME>find_nearest.csv
# echo Phase "," CPUs "," ALG ","ITERATION"," TIME"," CALLCOUNT"," SINGLETIME >inflate_obstacles.csv
# echo Phase "," CPUs "," ALG ","ITERATION"," TIME"," CALLCOUNT"," SINGLETIME>total.csv
for c in $CPUS; do
	export OMP_NUM_THREADS=$c
	export OMP_CANCELLATION=true
	for r in $(seq 1 $ITERATION); do
		./$BIN $MAP_SELECTION $r $c
	done
done
