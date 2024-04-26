#include <omp.h>
#include <iostream>
#include <stdio.h>
#include <unistd.h>

int main(){
    // omp_set_num_threads(6);
    #pragma omp parallel num_threads(4)
    {
        int thread_num =  omp_get_thread_num();
    
        // #pragma omp master
        // {
        //     printf("1 директива -> num_thread = %d \n", thread_num);
        // }

        // printf("ОБЩАЯ ЧАСТЬ -> num_thread = %d \n", thread_num);

        // #pragma omp single
        // {
        //     printf("2 директива -> num_thread = %d \n", thread_num);
        // }


        // #pragma omp sections
        // {
        //     #pragma omp section
        //     {
        //         printf("1 директива -> num_thread = %d \n", thread_num);
        //     }
        //     #pragma omp section
        //     {
        //         printf("2 директива -> num_thread = %d \n", thread_num);
        //     }
        // }
        // printf("ВЫХОД -> num_thread = %d \n", thread_num);
        int x = 0;
        #pragma omp for // shared(x)
            for(int i = 0; i < 10; ++i){
                x += i;
            }
        printf("%d" )


    }
    std::cerr << "\n";

    return 0;
}