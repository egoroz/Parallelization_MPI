#include <iostream>
#include <stdio.h>
#include <vector>
#include <cmath>
#include <mpi.h>

#define ERROR_CODE -1
// #define CHECK(w) do{CheckSuccess(w);} while(false)

int CheckSuccess(int success){
    if (MPI_SUCCESS != success){MPI_Abort(MPI_COMM_WORLD, ERROR_CODE); return ERROR_CODE;}
    return 0;
}

int main(int argc, char *argv[]) {

    CheckSuccess(MPI_Init(&argc, &argv));
    int rank, num_procs;
    CheckSuccess(MPI_Comm_rank(MPI_COMM_WORLD, &rank));
    CheckSuccess(MPI_Comm_size(MPI_COMM_WORLD, &num_procs));

    int start;
    int end;

  
    int N = 1000; // Default
    if (argc >= 2) {
        N = std::stoi(argv[1]);
    }

    if(rank == 0){
        std::cout << "Set N = " << N << '\n';
    }

    double eps = 1e-6;

    if (rank != num_procs - 1){
        start = rank * ((N - 1)/num_procs) + 1;
        end = (rank + 1) * ((N - 1)/num_procs);
    }

    if (rank == num_procs - 1){
        start = (rank) * ((N - 1)/num_procs) + 1;
        end = N - 1;
    }

    int space = end - start + 1; // размер зоны ответственности каждого процесса

    std::vector<double> u(N + 1, 0);
    std::vector<double> unew(N + 1, 0);

    u[0] = 1.0;
    unew[0] = 1.0;

    double h = 1.0 / N;
    double tau = 0.5 * h * h; 


    int count = 0;
    while (true) {
        for (int i = start; i < end + 1; ++i) {
            unew[i] = u[i] + (tau / (h * h)) * (u[i - 1] - 2 * u[i] + u[i + 1]);
        }

        if (rank == 0){
            CheckSuccess(MPI_Send(&unew[end], 1, MPI_DOUBLE, 1, 2, MPI_COMM_WORLD));
            CheckSuccess(MPI_Recv(&unew[end + 1], 1, MPI_DOUBLE, 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE));
        } else if(rank == num_procs - 1){
            CheckSuccess(MPI_Send(&unew[start], 1, MPI_DOUBLE, num_procs - 2, 1, MPI_COMM_WORLD));
            CheckSuccess(MPI_Recv(&unew[start - 1], 1, MPI_DOUBLE, num_procs - 2, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE));
        } else{
            CheckSuccess(MPI_Send(&unew[end], 1, MPI_DOUBLE, rank + 1, 2, MPI_COMM_WORLD));
            CheckSuccess(MPI_Recv(&unew[end + 1], 1, MPI_DOUBLE, rank + 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE));

            CheckSuccess(MPI_Send(&unew[start], 1, MPI_DOUBLE, rank - 1, 1, MPI_COMM_WORLD));
            CheckSuccess(MPI_Recv(&unew[start - 1], 1, MPI_DOUBLE, rank - 1, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE));
        }

        double local_max_delta = 0.0;
        for (int i = start; i < end + 1; i++) {
            double delta = std::fabs(unew[i] - u[i]);
            local_max_delta = std::max(local_max_delta, delta);
        }

        double global_max_delta;
        CheckSuccess(MPI_Allreduce(&local_max_delta, &global_max_delta, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD)); // Reduce + Bcast

        if (global_max_delta < eps) {
            break;
        }

        for (int i = start - 1; i < end + 2; ++i){
            u[i] = unew[i];
        }

        count++;
    }

    if (rank != 0){
        CheckSuccess(MPI_Gather(&start, 1, MPI_INT, nullptr, 1, MPI_INT, 0, MPI_COMM_WORLD));
        CheckSuccess(MPI_Gather(&space, 1, MPI_INT, nullptr, 1, MPI_INT, 0, MPI_COMM_WORLD));
        CheckSuccess(MPI_Gatherv(&unew[start], space, MPI_DOUBLE, nullptr, nullptr, nullptr, MPI_DOUBLE, 0, MPI_COMM_WORLD));
    }

    if (rank == 0) {
        std::vector<int> counts(num_procs, 0);
        std::vector<int> displs(num_procs, 0);

        displs[0] = start;
        counts[0] = space;

        CheckSuccess(MPI_Gather(MPI_IN_PLACE, 1, MPI_INT, displs.data(), 1, MPI_INT, 0, MPI_COMM_WORLD));
        CheckSuccess(MPI_Gather(MPI_IN_PLACE, 1, MPI_INT, counts.data(), 1, MPI_INT, 0, MPI_COMM_WORLD));
        CheckSuccess(MPI_Gatherv(MPI_IN_PLACE, space, MPI_DOUBLE, unew.data(), counts.data(), displs.data(), MPI_DOUBLE, 0, MPI_COMM_WORLD));
        
        std::cout << "Total steps = " << count << '\n';

        FILE *ff;

        if ((ff = fopen("par_res", "w+")) == NULL){
            printf("Can't open file\n");
            MPI_Abort(MPI_COMM_WORLD, -1);
        }

        for(int i=0; i<N+1; i++){
            fprintf(ff, "%f\n", unew[i]);
        }

        fclose(ff);
    }

    CheckSuccess(MPI_Finalize());
    return 0;
}