#!/bin/bash

docker run -v $PWD:/sandbox:Z abouteiller/mpi-ft-ulfm mpicc main.c -o test
docker run -v $PWD:/sandbox:Z abouteiller/mpi-ft-ulfm mpirun --map-by :oversubscribe --mca btl tcp,self --with-ft ulfm -np 5 a.out 10 