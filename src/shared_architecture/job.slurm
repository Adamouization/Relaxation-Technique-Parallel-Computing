#!/bin/bash
#SBATCH --account=cm30225
#SBATCH --partition=teaching
#SBATCH --job-name=aj645_cw1
#SBATCH --output=relaxation.%j.out
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=16
#SBATCH --mail-type=END
#SBATCH --mail-user=aj645@bath.ac.uk
./relaxation $1
