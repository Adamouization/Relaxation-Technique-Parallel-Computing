# Parallel Computing Relaxation Technique in a Shared Memory Architecture

## Problem Description

The objective of this assignment use low-level primitive parallelism constructs on a shared memory architecture while getting a feel for how parallel problems scale on such architectures. The goal is to implement relaxation using C and pthreads on Balena (the University of Bath's mid-sized cluster) by repeatedly averaging sets of four numbers. 

The **relaxation technique** is a solution to differential equations. It is done by having an array of values and repeatedly replacing a value with the average of its four neighbours; excepting boundary values, which remain at fixed values. This is repeated until all values settle down to within a given precision.

The solver expects:
* an initial square array of double values,
* an integer dimension (the dimension of the array),
* the number of threads to use,
* and a precision to work to.

The edges of the array form the boundary, values there are fixed, so the averaging will continue to iterate until the new values of all
elements differ from the previous ones by less than the precision. The code runs on varying sizes of arrays and varying numbers of cores in a shared memory configuration.

## Usage

### On a local machine using GCC

* Compile using `make` or `gcc main.c array_helpers.c print_helpers.c -o main.exe -pthread -Wall -Wextra -Wconversion`

* Run: `./main`

* Clean output files: `make clean`

### On the Balena cluster using SLURM

* SSH into Balena: `ssh [user_name]@balena.bath.ac.uk`
* `cd` into the project directory and submit the SLURM job script [`job.slurm`](https://github.com/Adamouization/Parallel-Computing-Relaxion-Shared-Memory/blob/master/job.slurm) to the queue: `sbatch jobscript.slurm`
* Monitor the job in the queue: `squeue -u [user_name]`
* View the results in the `relaxation.<job_id>.out` file using the `cat` command.

### Copying files to/from Balena

* from BUCS to Balena: `cp $BUCSHOME/dos/year\ 4/CM30225/Parallel-Computing-Relaxion-Shared-Memory/<file> /home/o/aj645/scratch/cw1-shared-architecture/`
* from Balena to BUCS: `cp /home/o/aj645/scratch/cw1-shared-architecture/<file> $BUCSHOME/dos/year\ 4/CM30225/Parallel-Computing-Relaxion-Shared-Memory/`

## TODO

See [TODO.md](https://github.com/Adamouization/Parallel-Computing-Relaxion-Shared-Memory/blob/master/TODO.md)
