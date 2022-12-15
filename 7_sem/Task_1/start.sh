mpicc main.c -o test -lm
mpiexec --oversubscribe -n $1 ./test
