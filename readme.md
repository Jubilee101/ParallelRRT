# How to use parallelRRT
make sure which version you wanna do:
1) serial version zixin/final-serial
2) parallel version zixin/final

git checkout [branch_name]
### 1. make
### 2. You can either choose to use ./bench.sh script


#### a. ./bench.sh 1/2/3 
#### the parameter is the map we want to use, 1 ->map.pgm, 2->office.pgm, 3->labyrinth.pgm

#### b. ./build/rrt [map] [iteration] [thread_num(only valid in bash setting)]
#### if you run this make sure you have folder exp_results, otherwise you cannot get the output file
#### e.g. ./build/rrt 1 10 


## !pay attention: if you wanna do the serial version, please checkout to another branch called zixin/final-serial

