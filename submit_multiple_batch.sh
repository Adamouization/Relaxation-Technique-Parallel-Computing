#!/bin/sh

# Submits 16 batch jobs to Balena in one go.
# Increments by the first number (5) in the z expression for every iteration, 
# which corresponds to the number of threads to run with, starting with the 
# second number (20)
 
for i in 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15
do
	# increment 1 by 1, starting from 1
    n=`expr $i \* 1 + 1`
	# increment 5 by 5, starting from 20
	#n=`expr $i \* 5 + 20`
    echo "Looping ... number $n"
    sbatch job.slurm $n A
done
