#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <mpi.h>

void prt1a(char *t1, double *v, int n, char *t2);

void print_matrix(double *a);

int N;
double *A;
#define A(i, j) A[(i) * (N + 1) + (j)]
double *X;
int proc_num, myrank;

int main(int argc, char **argv)
{
	MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &proc_num);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

	int i, j, k;
	N = atoi(argv[1]);

	/* create arrays */
	A = (double *)malloc(N * (N + 1) * sizeof(double));
	X = (double *)malloc(N * sizeof(double));
	if (myrank == 0) printf("GAUSS %dx%d\n----------------------------------\n", N, N);

	srand(12345);
	/* initialize array A*/
	for (i = 0; i <= N - 1; i++)
		for (j = 0; j <= N; j++)
			if (i == j || j == N)
				A(i, j) = 1.f;
			else
				A(i, j) = 0.f;

	double time0 = MPI_Wtime();
	/* elimination */
	for (i = 0; i < N - 1; i++)
	{
		MPI_Bcast(&A(i, i + 1), N - i, MPI_DOUBLE, i % proc_num, MPI_COMM_WORLD);
		for (k = myrank; k <= N - 1; k += proc_num) {
			if (k < i + 1) {
				continue;
			}
			for (j = i + 1; j <= N; j++) {
				A(k, j) = A(k, j) - A(k, i) * A(i, j) / A(i, i);
			}
		}
	}
	/* reverse substitution */
	X[N - 1] = A(N - 1, N) / A(N - 1, N - 1);
	for (j = N - 2; j >= 0; j--)
	{
		for (k = myrank; k <= j; k += proc_num)
			A(k, N) = A(k, N) - A(k, j + 1) * X[j + 1];
		MPI_Bcast(&A(j, N), 1, MPI_DOUBLE, j % proc_num, MPI_COMM_WORLD);
		X[j] = A(j, N) / A(j, j);
	}
	double time1 = MPI_Wtime();

	if (myrank == 0) {
		printf("Time in seconds=%gs\n", time1 - time0);
		prt1a("X=(", X, N > 9 ? 9 : N, "...)\n");
	}
	free(A);
	free(X);
	return 0;
}

void prt1a(char *t1, double *v, int n, char *t2)
{
	int j;
	printf("%s", t1);
	for (j = 0; j < n; j++)
		printf("%.4g%s", v[j], j % 10 == 9 ? "\n" : ", ");
	printf("%s", t2);
} 

void print_matrix(double* a) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N + 1; j++)
            printf("%lf ", A(i, j));
        printf("\n");
    }
    printf("\n");
}