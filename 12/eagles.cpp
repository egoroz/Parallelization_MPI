#include <iostream>
#include <omp.h>
#include <unistd.h>
#include <stdio.h>

int main(){
    static omp_lock_t lock;
    omp_init_lock(&lock);
    static int rows = 5;
    static int count_meals = 2;
    #pragma omp parallel num_threads(4)
    {
        int eagle_num = omp_get_thread_num();
        
        if(eagle_num == 0){
            int flights = 1;
            while(flights < count_meals){
        // std::cerr << "HERE0\n";
                if(rows == 0){
                    omp_set_lock(&lock);
                    printf("eagle flew out of eaglets \n");
                    sleep(3);
                    ++flights;
                    rows += 5;
                    printf("eagle bring 5 rows \n");
                    omp_unset_lock(&lock);
                }
                // printf("0: flights = %d, rows = %d \n", flights, rows);
            }
        }
        else if(eagle_num != 0){
            int eaten = 0;
            while(eaten <= count_meals){
            // std::cerr << "HERE1\n";
                if(rows > 0){
                omp_set_lock(&lock);
                    if(rows != 0){
                    printf("eaglet %d begin eat row \n", eagle_num);
                    --rows;
                    sleep(1);
                    ++eaten;
                    printf("eaglet %d end eat row \n", eagle_num);
                    }
                omp_unset_lock(&lock);
                    
                } else {
                    printf("AAAAA from eaglet %d \n", eagle_num);
                    sleep(3);
                }
            }
        }
    }

    omp_destroy_lock(&lock);

    return 0;
}