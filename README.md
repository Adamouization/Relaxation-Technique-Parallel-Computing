# Parallel Computing Relaxation Technique in a Shared Memory Architecture

## Problem Description

The objective of this assignment is to give you experience of using low-level primitive parallelism constructs on a
shared memory architecture; plus a feel for how parallel problems scale on such architectures.

The background is the solution of differential equations using a method called the relaxation technique. This is done by
having an array of values and repeatedly replacing a value with the average of its four neighbours; excepting boundary
values, which remain at fixed values. This is repeated until all values settle down to within a given precision.

The goal is to implement relaxation, using C and pthreads on Balena by repeatedly averaging sets of four numbers.

The solver expects:
* an initial square array of double values,
* an integer dimension (the dimension of the array),
* the number of threads to use,
* and a precision to work to.

The edges of the array form the boundary, values there are fixed, so the averaging will continue to iterate until the new values of all
elements differ from the previous ones by less than the precision.

The code should run on varying sizes of arrays and varying numbers of cores in a shared memory configuration.

## Usage

### On a local machine using GCC

```
gcc main.c -o main.exe -lpthread -Wall -Wextra -Wconversion
./main.exe
```

### On Balena (University of Bath's medium-sized cluster) using SLURM

* SSH into Balena: `ssh [user_name]@balena.bath.ac.uk`
* `cd` into the project directory and create a `sbatch` job submission script.
* Submit the job to the queue: `sbatch jobscript.slurm`
* Monitor the job in the queue: `squeue -u [user_name]`

### Copying files to/from Balena

* from BUCS to Balena: `cp $BUCSHOME/dos/Computer\ Science\ Degree/year\ 4/CM30225/Parallel-Computing-Relaxion-Shared-Memory/main.c /home/o/aj645/scratch/cw1-shared-architecture/`
* from Balena to BUCS: `cp /home/o/aj645/scratch/cw1-shared-architecture/job.slurm $BUCSHOME/dos/Computer\ Science\ Degree/year\ 4/CM30225/Parallel-Computing-Relaxion-Shared-Memory/`

## TODO

See [TODO.md](https://github.com/Adamouization/Parallel-Computing-Relaxion-Shared-Memory/blob/master/TODO.md)
