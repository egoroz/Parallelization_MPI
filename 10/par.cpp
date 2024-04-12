#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include<mpi.h>
#include<iostream>

int ERROR_CODE = 1;

int check(int success){
    ++ERROR_CODE;
    if (MPI_SUCCESS != success){MPI_Abort(MPI_COMM_WORLD, ERROR_CODE); return ERROR_CODE;}
    return 0;
}

int main(int argc, char *argv[]){

    double *u, *unew, delta, maxdelta;
    double eps=1.e-6;
    double h, tau;

    int N;
    int i;
    int count = 0;

    check(MPI_Init(&argc, &argv));

    int size;
    int rank;
    int start;
    int end;
    int zone;
    MPI_Status st;

    check(MPI_Comm_size(MPI_COMM_WORLD, &size));

    check(MPI_Comm_rank(MPI_COMM_WORLD, &rank));

    FILE *ff;

    // if(argc !=2 ){
    //     printf("Usage: exefile npoints\n");
    //     exit(-1);
    // }

    // N = atoi(argv[1]);
    N = 1000;
    if (N == 0) {
        printf("Set N to 1000\n");
        N =1000;
    } else {
        printf("Set N to %d\n", N);
    }


    if (rank != size - 1){
        start = rank * ((N - 1) / size) + 1;
        end = (rank + 1) * ((N - 1) / size);
        zone = end - start + 1;
    } else{
        start = rank * ((N - 1) / size) + 1;
        end = N-1;
        zone = end - start + 1;      
    }

    if((u = (double*) malloc((N + 1)*sizeof(double))) == NULL){
        printf("Can't allocate memory for u\n");
        MPI_Abort(MPI_COMM_WORLD, 100);
    }

    if((unew = (double*) malloc((N + 1)*sizeof(double))) == NULL){
        printf("Can't allocate memory for unew\n");
        free(u);
        MPI_Abort(MPI_COMM_WORLD, 100);
    }    


    for(i=1; i<N; i++){
        u[i] = 0;
    }

    unew[0] = u[0] = 1.0;
    unew[N] = u[N] = 0;
    

    h = 1.0/N;
    tau = 0.5*(h*h);

    while(1){
        for (i=start; i < end + 1; i++){
           unew[i] = u[i]+(tau/(h*h))*(u[i-1]-2*u[i]+u[i+1]);
        }

        maxdelta = 0;
        for (i=start; i < end  + 1; i++){
            delta = fabs(unew[i]-u[i]);
            if(delta > maxdelta) {
                maxdelta = delta;
            }
        }

        if (rank != 0){
            check(MPI_Reduce(&maxdelta, nullptr, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD));
        } else {
            check(MPI_Reduce(MPI_IN_PLACE, &maxdelta, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD));
        }

        check(MPI_Bcast(&maxdelta, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD));
        // if(rank == 0){
        //     std::cout << maxdelta << '\n';
        // }
        if (maxdelta < eps){
            break;
        }
        count++;

        // if (rank == 0){
        //     for (int i=start; i < end + 1; ++i){
        //         std:: cout << unew[i] << ' ';
        //     }
        //     std::cout << '\n';
        // }

        if (rank == 0){
            check(MPI_Send(&unew[end], 1, MPI_DOUBLE, 1, 2, MPI_COMM_WORLD));
            check(MPI_Recv(&unew[end + 1], 1, MPI_DOUBLE, 1, 1, MPI_COMM_WORLD, &st));
        } else if (rank == size - 1) {
            check(MPI_Send(&unew[start], 1, MPI_DOUBLE, size - 2, 1, MPI_COMM_WORLD));
            check(MPI_Recv(&unew[start - 1], 1, MPI_DOUBLE, size - 2, 2, MPI_COMM_WORLD, &st));
        } else {
            check(MPI_Send(&unew[end], 1, MPI_DOUBLE, rank + 1, 2, MPI_COMM_WORLD));
            check(MPI_Recv(&unew[end + 1], 1, MPI_DOUBLE, rank + 1, 1, MPI_COMM_WORLD, &st));

            check(MPI_Send(&unew[start], 1, MPI_DOUBLE, rank - 1, 1, MPI_COMM_WORLD));
            check(MPI_Recv(&unew[start - 1], 1, MPI_DOUBLE, rank - 1, 2, MPI_COMM_WORLD, &st));

            // if (rank == 1){
            //     std:: cout << unew[start] << ' ' << unew[end + 1] << '\n';
            // }            
        }
        for(i = start -1; i < end + 2; i++){
            u[i] = unew[i];
        }
    }



    if (rank != 0){
        check(MPI_Gather(&start, 1, MPI_INT, nullptr, 1, MPI_INT, 0, MPI_COMM_WORLD));
        check(MPI_Gather(&zone, 1, MPI_INT, nullptr, 1, MPI_INT, 0, MPI_COMM_WORLD));
        check(MPI_Gatherv(&unew[start], zone, MPI_DOUBLE, nullptr, nullptr, nullptr, MPI_DOUBLE, 0, MPI_COMM_WORLD));
    }

    if (rank == 0){
        for (int i=start; i < end + 1; ++i){
            // std:: cout << unew[i] << ' ';clear
        }
        // std::cout << '\n';
        int displs[size];
        int counts[size];
        displs[0] = start;
        counts[0] = zone;
        check(MPI_Gather(MPI_IN_PLACE, 1, MPI_INT, displs, 1, MPI_INT, 0, MPI_COMM_WORLD));
        check(MPI_Gather(MPI_IN_PLACE, 1, MPI_INT, counts, 1, MPI_INT, 0, MPI_COMM_WORLD));
        check(MPI_Gatherv(MPI_IN_PLACE, zone, MPI_DOUBLE, unew, counts, displs, MPI_DOUBLE, 0, MPI_COMM_WORLD));
    

        printf("%d steps\n",count);
        if ((ff = fopen("parres", "w+")) == NULL){
        printf("Can't open file\n"); free(u); free(unew);
        MPI_Abort(MPI_COMM_WORLD, 100);
        }

        for(i=0; i<N+1; i++)
        fprintf(ff, "%f\n", unew[i]);

        fclose(ff); free(u); free(unew);

    }

    MPI_Finalize();
    return 0;
    }