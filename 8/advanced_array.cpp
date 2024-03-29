#include <iostream>
#include <fstream>
#include <vector>
#include <mpi.h>

int ERROR_CODE = 1;

void CheckSuccess(int success){
    ERROR_CODE++;
    if (MPI_SUCCESS != success){MPI_Abort(MPI_COMM_WORLD, ERROR_CODE);}
}

int main(int argc, char** argv) {
    CheckSuccess(MPI_Init(&argc, &argv));

    int rank, size;
    CheckSuccess(MPI_Comm_rank(MPI_COMM_WORLD, &rank));
    CheckSuccess(MPI_Comm_size(MPI_COMM_WORLD, &size));

    std::vector<int> local_data(rank + 1, rank + 1);

    std::vector<int> recv_counts(size);
    for (int i = 0; i < size; ++i) {
        recv_counts[i] = i + 1;
    }


    std::vector<int> displacements(size);
    displacements[0] = 0;
    for (int i = 1; i < displacements.size(); ++i) {
        displacements[i] = displacements[i-1] + 2 * i;
    }

    if (rank != 0){
        CheckSuccess(MPI_Gatherv(local_data.data(), rank + 1, MPI_INT, nullptr, nullptr, nullptr, MPI_INT, 0, MPI_COMM_WORLD));
    }

    if (rank == 0) {
        int res_size = size * size;

        std::vector<int> result(res_size, 0);
        MPI_Gatherv(local_data.data(), 1, MPI_INT, result.data(), recv_counts.data(), displacements.data(), MPI_INT, 0, MPI_COMM_WORLD);
        std::ofstream output_file("output.txt");
        for (int i = 0; i < result.size(); ++i) {
            output_file << result[i] << " ";
        }
        output_file.close();
    }

    MPI_Finalize();
    return 0;
}
