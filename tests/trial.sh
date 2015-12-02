#!/bin/bash
SHELL_DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
cd $SHELL_DIR
mpiexec -np 3 ../build/bin/ComputeSubsample ./sample.json $1 $2 ./subsample_$1_$2.json
mpiexec -np 3 xterm -e lldb ../build/bin/ComputeDistances $3 ./subsample_$1_$2.json ./distance_$1_$2_$3.txt


