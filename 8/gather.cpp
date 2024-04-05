#include <iostream>
#include <fstream>
#include <vector>
#include <mpi.h>

// ЗАДАЧА: Сбор данных MPI_Gather

int ERROR_CODE = 1;

void CheckSuccess(int success){
    ERROR_CODE++;
    if (MPI_SUCCESS != success){MPI_Abort(MPI_COMM_WORLD, ERROR_CODE);}
}

int main(int argc, char **argv){
    CheckSuccess(MPI_Init(&argc, &argv));

    int size;
    CheckSuccess(MPI_Comm_size(MPI_COMM_WORLD, &size));

    int rank;
    CheckSuccess(MPI_Comm_rank(MPI_COMM_WORLD, &rank));

    const int N = 120;

    if (N % size != 0) {
            MPI_Finalize();
            std::cout << ("Некорректные данные");
            return -1;
    }

    const int local_N = N / size;
    std::vector<int> local_elements(local_N, rank);
    std::vector<int> all_elements(N);

    CheckSuccess(MPI_Gather(local_elements.data(), local_N, MPI_INT, all_elements.data(), local_N, MPI_INT, 0, MPI_COMM_WORLD));
    

    if (rank == 0) {
        std::ofstream output_file("output.txt");
        if (output_file.is_open()) {
            for (int i = 0; i < N; ++i) {
                output_file << all_elements[i];
                if (i != N - 1) {
                    output_file << " ";
                }
            }
            output_file.close();
            std::cout << "Результат записан в output.txt" << std::endl;
        } else {
            std::cerr << "Ошибка при открытии файла для записи." << std::endl;
        }
    }

    
    MPI_Finalize();
    return 0;
}