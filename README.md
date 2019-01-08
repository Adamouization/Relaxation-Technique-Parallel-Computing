# Relaxation Technique Parallelised in C using Pthreads and MPI

## Problem Description

The objective of this assignment is to use low-level primitive parallelism constructs, first on a shared memory architecture then on a distributed memory architecture, and analyse how parallel problems scale on such an architectures using C, pthreads and MPI on Balena, a mid-sized cluster with 2720 cpu cores.

The background is a method called relaxation technique, a solution to differential equations, which is achieved by having a square array of values and repeatedly replacing a value with the average of its four neighbours, excepting boundaries values which remain fixed. This process is repeated until all values settle down to within a given precision.

## Usage

### Shared Memory Architecture (pthreads)

* Compile using the makefile: `make`, or `gcc main.c array_helpers.c print_helpers.c -o shared_relaxation -pthread -Wall -Wextra -Wconversion`
* Run: `./shared_relaxation <number_of_threads>`

### Distributed Memory Architecture (MPI)

* Compile using the makefile: `make` or: `mpicc -Wall -Wextra -Wconversion main.c -o distributed_relaxation -lm`
* Run: `mpirun -np <num_processes> ./distributed_relaxation -d <dimension> -p <precision> -debug <debug mode>`

where:
* -np corresponds to the number of processes;
* -d corresponds to the dimensions of the square array;
* -p corresponds to the precision of the relaxation;
* -debug corresponds to the debug mode (0: only essential information, 1: row allocation logs, 2: process IDs logs, 3: initial and nal arrays, 4: iteration debugging data).

### Other

#### Running the shared memory architecture on the Balena cluster using SLURM

* SSH into Balena: `ssh [user_name]@balena.bath.ac.uk`
* `cd` into the project directory and submit the SLURM job script [`job.slurm`](https://github.com/Adamouization/Parallel-Computing-Relaxion-Shared-Memory/blob/master/job.slurm) to the queue: `sbatch jobscript.slurm`
* Monitor the job in the queue: `squeue -u [user_name]`
* View the results in the `relaxation.<job_id>.out` file using the `cat *.out` command.

#### Submitting multiple files using a bash script (shared memory testing)

* `./submit_multiple_batch 1`
* `cat *.out`
* Use following regex to retrieve time by using this regex `\=.(\d+)\..(\d+)` and pasting the output in [regex101](https://regex101.com/)

#### Copying files to/from Balena

* from BUCS to Balena: `cp $BUCSHOME/dos/year\ 4/Relaxation-Technique-Parallel-Computing/src/<file> /home/o/aj645/scratch/cw2-distributed-architecture/`

* from Balena to BUCS: `cp /home/o/aj645/scratch/cw2-distributed-architecture/<file> $BUCSHOME/dos/year\ 4/Relaxation-Technique-Parallel-Computing`

## Reports

* You can read the shared architecture implementation using pthreads report [here](https://github.com/Adamouization/Relaxation-Technique-Parallel-Computing/blob/master/reports/shared_architecture_report.pdf).
* You can read the distributed architecture implementation using MPI report [here](https://github.com/Adamouization/Relaxation-Technique-Parallel-Computing/blob/master/reports/distributed_architecture_report.pdf).

## TODO

See [TODO.md](https://github.com/Adamouization/Relaxation-Technique-Parallel-Computing/blob/master/TODO.md)

## License 
* see [LICENSE](https://github.com/Adamouization/Relaxation-Technique-Parallel-Computing/blob/master/LICENSE) file.

## Contact
* email: adam@jaamour.com
* website: www.adam.jaamour.com
* twitter: [@Adamouization](https://twitter.com/Adamouization)