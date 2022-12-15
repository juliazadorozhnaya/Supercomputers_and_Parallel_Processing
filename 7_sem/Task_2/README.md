```
docker pull abouteiller/mpi-ft-ulfm - will pull all needed libs an env

./build.sh - to construct project

./qstart.sh - to start test with prewritten params, such as base process number = 5, matrix size 10X10

./start.sh -np  "num_proc" a.out "matrix_size(one param)" "number of additional proc, default = 2" - to start customizible test
```