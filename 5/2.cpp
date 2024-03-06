#include <iostream>
#include <mpi.h>

// ЗАДАЧА: заставить процессы обмениваться сообщениями по кругу. В сообщении должно содержаться число, каждый процесс прибавляет полученное число к своему номеру и передает его следующему.

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

    int i = 0;  // Это передает
    int j = 0;  // Это получает
    MPI_Status mpi_status;

    if (rank == 0){  // Отдельно смотрим первую отправку
    CheckSuccess(MPI_Send(&i, 1, MPI_INT, (rank + 1)%size, rank, MPI_COMM_WORLD));
    std::cout << "Я rank = " << rank << ". Отправил на rank = " << (rank + 1)%size << " значение = " << i << "\n";
    }

    int d = rank - 1;  // Нужна для корректного перехода через 0 от size-1
    if (rank == 0){d = size - 1;}

    CheckSuccess(MPI_Recv(&j, 1, MPI_INT, d, d, MPI_COMM_WORLD, &mpi_status));
    std::cout << "Я rank = " << rank << ". Принял от "<< d <<" что  = " << j << "\n";

    i = j + rank;

    CheckSuccess(MPI_Send(&i, 1, MPI_INT, (rank + 1)%size, rank, MPI_COMM_WORLD));
    std::cout << "Я rank = " << rank << ". Отправил на rank = " << (rank + 1)%size << " значение = " << i << "\n";

    
    MPI_Finalize();
    return 0;
}