#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include <mpi.h>
#include <unistd.h>

MPI_Request perform_operation(int x, int y) {
    return x > y ? x : y;
}

MPI_Request send_int(int x, int y, MPI_Comm comm, int *data) {
    int target_rank;
    int coords[2] = {x, y};
    MPI_Cart_rank(comm, coords, &target_rank);

    MPI_Request request;
    MPI_Isend(data, 1, MPI_INT, target_rank,
              0, MPI_COMM_WORLD, &request);
    return request;
}

int receive_int(int x, int y, MPI_Comm comm) {
    int target_rank;
    int coords[2] = {x, y};
    MPI_Cart_rank(comm, coords, &target_rank);

    int data;
    MPI_Recv(&data, 1, MPI_INT, target_rank,
             MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    return data;
}

int main(int argc, char **argv) {
    int matrix_size;

    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &matrix_size);

    matrix_size = (int) sqrt((double) matrix_size);

    int rank, size;

    int ndims = 2;

    int n_size[2] = {matrix_size, matrix_size};
    int periodic[2] = {0, 0};

    MPI_Comm cart_comm;

    MPI_Cart_create(MPI_COMM_WORLD, ndims, n_size, periodic, 1, &cart_comm);

    MPI_Comm_rank(cart_comm, &rank);
    MPI_Comm_size(cart_comm, &size);

    srand(time(NULL) + rank);

    int coords[2];
    MPI_Cart_coords(cart_comm, rank, ndims, coords);

    int my_number = rand() % 128;

    printf("My number is %4d, my coords is <%2d, %2d>\n", my_number, coords[0], coords[1]);
    sleep(1);

    int number;
    if (coords[0] == 0) {
        number = receive_int(coords[0] + 1, coords[1], cart_comm);
        my_number = perform_operation(my_number, number);
        printf("Got %d, my coords is <%2d, %2d>\n", number, coords[0], coords[1]);


        if (coords[1] + 1 != matrix_size) {
            number = receive_int(coords[0], coords[1] + 1, cart_comm);
            my_number = perform_operation(my_number, number);
            printf("Got %d, my coords is <%2d, %2d>\n", number, coords[0], coords[1]);
        }

        if (coords[1] != 0) {
            MPI_Request request;
            request = send_int(coords[0], coords[1] - 1, cart_comm, &my_number);
            MPI_Wait(&request, MPI_STATUS_IGNORE);
        }
    } else {
        if (coords[0] + 1 != matrix_size) {
            number = receive_int(coords[0] + 1, coords[1], cart_comm);
            my_number = perform_operation(my_number, number);

            printf("Got %d, my coords is <%2d, %2d>\n", number, coords[0], coords[1]);
        }

        MPI_Request request;
        request = send_int(coords[0] - 1, coords[1], cart_comm, &my_number);
        MPI_Wait(&request, MPI_STATUS_IGNORE);
    }

    sleep(1);

    if (coords[0] == 0 && coords[1] == 0) {
        printf("Total number is %d\n", my_number);
    }


    MPI_Barrier(cart_comm);
    sleep(1);

    if (coords[0] == 0) {
        if (coords[1] != 0) {
            my_number = receive_int(coords[0], coords[1] - 1, cart_comm);
            printf("Got number, my coords is <%2d, %2d>\n", coords[0], coords[1]);
        }

        MPI_Request r1, r2;
        if (coords[1] + 1 != matrix_size) {
            r1 = send_int(coords[0], coords[1] + 1, cart_comm, &my_number);
        }
        r2 = send_int(coords[0] + 1, coords[1], cart_comm, &my_number);
        if (coords[1] + 1 != matrix_size) {
            MPI_Wait(&r1, MPI_STATUS_IGNORE);
        }
        MPI_Wait(&r2, MPI_STATUS_IGNORE);
    } else {
        my_number = receive_int(coords[0] - 1, coords[1], cart_comm);
        printf("Got number, my coords is <%2d, %2d>\n", coords[0], coords[1]);
        MPI_Request r1;
        if (coords[0] + 1 != matrix_size) {
            r1 = send_int(coords[0] + 1, coords[1], cart_comm, &my_number);
            MPI_Wait(&r1, MPI_STATUS_IGNORE);
        }
    }

    MPI_Finalize();

    return 0;
}