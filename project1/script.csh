

#!/bin/bash
# number of threads
for t in 1 2 4 6 8 10
do
    echo NUMT = $t
    for a in 1000000 5000000 10000000 20000000 40000000 60000000 80000000 10000$
    do
      	g++ -DNUMT=$t -DNUMTRIALS=$a  project1.cpp -o proj -lm -fopenmp
        ./proj
    done
done