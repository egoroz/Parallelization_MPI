#include <omp.h>
#include <iostream>
#include <fstream>
#include <cmath>

double f(double x){
    return sqrt(4.0 - x*x);
}

int main(){
    int num_procs = omp_get_num_procs();

    double result = 0;
    int N = 1000;  // default value

    std::ifstream data("N.dat");
    if(data.is_open()){data >> N;}
    
    double h = 2.0 / N;

    #pragma omp parallel num_threads(num_procs) reduction(+:result)
    {
        int num_threads = omp_get_num_threads();
        int thread_num = omp_get_thread_num();
        
        #pragma omp for
            for (int i = 0; i < N; ++i) {
                result += (f(i * h) + f((i + 1) * h)) * h / 2.0;
            }
    }

    printf("pi = %.13lf\n", result);

    return 0;
}