#!/bin/bash
#SBATCH --account=cm30225
#SBATCH --partition=teaching
#SBATCH --job-name=aj645_cw2
#SBATCH --output=relaxation.%j.out
#SBATCH --nodes=4
#SBATCH --ntasks-per-node=16
#SBATCH --mail-type=END
#SBATCH --mail-user=aj645@bath.ac.uk
module load intel/mpi
mpirun -np 16 ./distributed_relaxation -d 2048 -p 0.001 -debug 0
