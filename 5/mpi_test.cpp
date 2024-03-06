#include <iostream>
#include <mpi.h>

// void CHECK_SUCCESS(const int &  success){

// }

int main(int argc, char **argv){
    int success = MPI_Init(&argc, &argv);
    int errorcode = 1;
    if (MPI_SUCCESS != success){MPI_Abort(MPI_COMM_WORLD, errorcode);}

    int size;
    if (MPI_SUCCESS != MPI_Comm_size(MPI_COMM_WORLD, &size)){MPI_Abort(MPI_COMM_WORLD, errorcode);};

    int rank;
    if (MPI_SUCCESS != MPI_Comm_rank(MPI_COMM_WORLD, &rank)){MPI_Abort(MPI_COMM_WORLD, errorcode);};

    std::cout << "rank = " << rank << " size = " << size << "\n";

    MPI_Finalize();
    return 0;
}