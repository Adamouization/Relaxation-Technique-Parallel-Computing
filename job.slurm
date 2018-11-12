#!/bin/bash
#SBATCH --account=cm30225
#SBATCH --partition=teaching
#SBATCH --job-name=aj645_cw1
#SBATCH --output=main.out
#SBATCH --error=main.err
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=16
./main.exe
