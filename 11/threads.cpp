#include <omp.h>
#include <stdio.h>

int main(){
    static int x0 = 5;
    omp_set_num_threads(6);
    #pragma omp threadprivate(x0) 
    #pragma omp parallel num_threads(2) copyin(x0) reduction(+:sum)
    {
        x0 = omp_get_thread_num();
        printf("PAR 1 -> num = %d, x0 = %d\n", omp_get_thread_num(), x0);

    }

    #pragma omp parallel num_threads(2)
    {
        x0 += 1;
        printf("PAR 2 -> num = %d, x0 = %d\n", omp_get_thread_num(), x0);

    }


    printf("SEQ2 -> num = %d, x0 = %d\n",  omp_get_thread_num(), x0);
    return 0;
}