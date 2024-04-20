#include <omp.h>
#include <iostream>
#include <cmath>

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

int main(){

    double result = 0;
    // #pragma omp threadprivate(result) 

    int num_procs = omp_get_num_procs();

    #pragma omp parallel num_threads(num_procs) reduction(+:result)
    {
        int num_threads = omp_get_num_threads();  // кол-во нитей
        int thread_num = omp_get_thread_num();
        result = calc(100000, thread_num, num_threads);
        // std::cout << "result_i = " << result << "\n";
    }

    std::cout << "RESULT PI = " << result << "\n";
    return 0;
}