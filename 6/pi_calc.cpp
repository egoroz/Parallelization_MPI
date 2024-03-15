#include <iostream>
#include <cmath>
#include <fstream>
#include <mpi.h>

// ЗАДАЧА: вычислить число пи с какой-то точностью, используя MPI

int ERROR_CODE = 1;

void CheckSuccess(int success){
    ERROR_CODE++;
    if (MPI_SUCCESS != success){MPI_Abort(MPI_COMM_WORLD, ERROR_CODE);}
}


double f(double x){
    return sqrt(4.0 - x*x);
}

double calc(int N, int rank, int size){  // Вычисление площади под графиком функци f
    double h = 2.0 / N;
    double S = 0;
    int start = rank*(N/size);
    if(rank != size - 1) {
        for (int i = start; i < (rank+1)*(N/size); i++) {
            S += (f(i * h) + f((i + 1) * h)) * h / 2.0;
	}
    } 
    else {
	    for (int i = start; i < N; i++) {
	        S += (f(i * h) + f((i + 1) * h)) * h / 2.0;
	    } 
    }
    
    return S;
}


int main(int argc, char **argv){
    CheckSuccess(MPI_Init(&argc, &argv));

    int size;
    CheckSuccess(MPI_Comm_size(MPI_COMM_WORLD, &size));

    int rank;
    CheckSuccess(MPI_Comm_rank(MPI_COMM_WORLD, &rank));

    int N = 0;
    double result = 0;  // число пи
    double result_i = 0;

    MPI_Status mpi_status;

    if (rank == 0){
        int number;
        std::ifstream file("N.dat"); // Открываем файл для чтения
        if(file.is_open())
        file >> number;
        file.close(); // Закрываем файл


        CheckSuccess(MPI_Send(&number, 1, MPI_INT, 0, 0, MPI_COMM_WORLD));
        for (int i = 1; i < size; ++i){
            CheckSuccess(MPI_Send(&number, 1, MPI_INT, i, i, MPI_COMM_WORLD));
        }
    }


    if (rank != 0){
        CheckSuccess(MPI_Recv(&N, 1, MPI_INT, 0, rank, MPI_COMM_WORLD, &mpi_status));
        result_i = calc(N, rank, size);
        CheckSuccess(MPI_Send(&result_i, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD));
    
    }


    if (rank == 0){
        CheckSuccess(MPI_Recv(&N, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &mpi_status));
        result_i = calc(N, rank, size);
        result = result_i;
        for (int i = 1; i < size; ++i){
            CheckSuccess(MPI_Recv(&result_i, 1, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, &mpi_status));
            result += result_i;
        }
    }

    if (rank == 0){
        std::cout << "RESULT: PI = ";
        printf("%.9lf\n", result);
    }
    
    MPI_Finalize();
    return 0;
}