#!/bin/sh
 
for i in 2 3 4 5 6 7 8 9 10 11 12 13 14 15
do
    z=`expr $i \* 64 + 1`
    echo "Looping ... number $z"
    sbatch myJob.slurm $z A
done
