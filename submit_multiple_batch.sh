#!/bin/sh

# Submits 16 batch jobs to Balena in one go.
# Increments by the first number in the z expression for every iteration, which 
# corresponds to the number of threads to run with.
 
for i in 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15
do
    z=`expr $i \* 1 + 1`
    echo "Looping ... number $z"
    sbatch job.slurm $z A
done
