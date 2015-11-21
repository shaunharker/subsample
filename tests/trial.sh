#!/bin/bash
SHELL_DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
cd $SHELL_DIR
mpiexec -np 4 ../build/bin/ComputeSubsample ./sample.json $1 $2 $3 ./subsample_$1_$2.json
mpiexec -np 4 ../build/bin/ComputeDistances ./subsample_$1_$2.json ./distance_$1_$2.txt
