#include <iostream>
#include <fstream>
#include <vector>
#include <stdlib.h>
#include <cmath>
#include <mpi.h>

int ERROR_CODE = 1;

int CheckSuccess(int success){
    ++ERROR_CODE;
    if (MPI_SUCCESS != success){MPI_Abort(MPI_COMM_WORLD, ERROR_CODE); return ERROR_CODE;}
    return 0;
}

int main(int argc, char** argv) {
    CheckSuccess(MPI_Init(&argc, &argv));

    int rank, size;
    CheckSuccess(MPI_Comm_rank(MPI_COMM_WORLD, &rank));
    CheckSuccess(MPI_Comm_size(MPI_COMM_WORLD, &size));

    srandom(rank+1); // set seed
    int kol = random()%7+1;

    std::vector<double> local_data(kol);
    for (int i = 0; i < kol; ++i) {
        local_data[i] = sqrt(rank);
    }


    if (rank != 0){
        CheckSuccess(MPI_Gather(&kol, 1, MPI_INT, nullptr, 1, MPI_INT, 0, MPI_COMM_WORLD));
        CheckSuccess(MPI_Gatherv(local_data.data(), kol, MPI_DOUBLE, nullptr, nullptr, nullptr, MPI_DOUBLE, 0, MPI_COMM_WORLD));
    }

    if (rank == 0) {
        std::vector<int> recv_counts(size);
        CheckSuccess(MPI_Gather(&kol, 1, MPI_INT, recv_counts.data(), 1, MPI_INT, 0, MPI_COMM_WORLD));

        std::vector<int> displacements(size);
        displacements[0] = 0;
        for (int i = 1; i < displacements.size(); ++i) {
            displacements[i] = displacements[i-1] + recv_counts[i-1];
        }

        std::vector<double> result(displacements[size-1] + recv_counts[size-1]);
        MPI_Gatherv(local_data.data(), 1, MPI_DOUBLE, result.data(), recv_counts.data(), displacements.data(), MPI_DOUBLE, 0, MPI_COMM_WORLD);
        std::ofstream output_file("output.txt");
        for(int j = 0; j < size; ++j){
            for (int i = displacements[j]; i < displacements[j] + recv_counts[j]; ++i) {
                output_file << result[i] << " ";
            }
            output_file << "\n";
        }
        output_file.close();
    }

    MPI_Finalize();
    return 0;
}