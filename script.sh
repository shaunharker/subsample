#!/bin/bash
#Active comments for SGE
#$ -V
#$ -cwd
#$ -j y
#$ -S /bin/bash
#$ -pe orte 880

cd $ENV_MODELDIR
/opt/openmpi/bin/mpiexec --mca mpi_preconnect_mpi 1 -np $NSLOTS -x LD_LIBRARY_PATH ./Subsample /share/data/sharker/data/DownUp 52500 $DELTA
