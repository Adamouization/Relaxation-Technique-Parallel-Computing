# Parallel Computing Relaxation Technique in a Shared Memory Architecture

## Problem Description

The objective of this assignment is to use low-level primitive parallelism constructs, first on a shared memory architecture then on a distributed memory architecture, and analyse how parallel problems scale on such an architectures using C, pthreads and MPI on Balena, a mid-sized cluster with 2720 cpu cores.

The background is a method called relaxation technique, a solution to differential equations, which is achieved by having a square array of values and repeatedly replacing a value with the average of its four neighbours, excepting boundaries values which remain fixed. This process is repeated until all values settle down to within a given precision.

## Usage

### Shared Memory Architecture (pthreads)

#### On a local machine using GCC

* Compile using `make` or `gcc main.c array_helpers.c print_helpers.c -o shared_relaxation -pthread -Wall -Wextra -Wconversion`

* Run: `./shared_relaxation <number_of_threads>`

* Clean output files: `make clean`

#### On the Balena cluster using SLURM

* SSH into Balena: `ssh [user_name]@balena.bath.ac.uk`
* `cd` into the project directory and submit the SLURM job script [`job.slurm`](https://github.com/Adamouization/Parallel-Computing-Relaxion-Shared-Memory/blob/master/job.slurm) to the queue: `sbatch jobscript.slurm`
* Monitor the job in the queue: `squeue -u [user_name]`
* View the results in the `relaxation.<job_id>.out` file using the `cat` command.

#### Submitting multiple files using a bash script

* `./submit_multiple_batch 1`
* `cat *.out`
* Use following regex to retrieve time by using this regex `\=.(\d+)\..(\d+)` and pasting the output in [regex101](https://regex101.com/)

### Distributed Memory Architecture (MPI)

* Compile using mpicc: `mpicc main.c -o distributed_relaxation -lm`
* Run the mpi program: `mpirun -np <number_of_processes> ./distributed_relaxation`

### Other

#### Copying files to/from Balena

* from BUCS to Balena: `cp $BUCSHOME/dos/year\ 4/CM30225/Parallel-Computing-Relaxion-Shared-Memory/<file> /home/o/aj645/scratch/cw1-shared-architecture/`
* from Balena to BUCS: `cp /home/o/aj645/scratch/cw1-shared-architecture/<file> $BUCSHOME/dos/year\ 4/CM30225/Parallel-Computing-Relaxion-Shared-Memory/`

## Report

You can read the final shared architecture implementation using pthreads report [here](https://github.com/Adamouization/Relaxation-Technique-Parallel-Computing/blob/master/reports/shared_architecture_report.pdf).

## TODO

See [TODO.md](https://github.com/Adamouization/Parallel-Computing-Relaxion-Shared-Memory/blob/master/TODO.md)
