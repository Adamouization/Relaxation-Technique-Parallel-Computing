#!/bin/sh
 
for i in 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15
do
z=`expr $i + 10`
    echo "$z"
    cat slurm-17410$z.out
done
