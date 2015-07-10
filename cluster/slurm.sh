#!/bin/bash
#SBATCH -n 10                 # One task
#SBATCH -c 1                 # One cpu per task
#SBATCH -N 1                 # Minimum one node
#SBATCH -t 0-00:05           # Runtime in D-HH:MM
#SBATCH -p main              # Partition to submit to
#SBATCH --mem-per-cpu=4000   # Memory pool for all cores (see also --mem-per-cpu)
#SBATCH --mail-type=ALL      # Type of email notification-BEGIN,END,FAIL,ALL
#SBATCH --mail-user=sharker81@gmail.com #Email to which notifications will be sent

mpiexec "$@"
