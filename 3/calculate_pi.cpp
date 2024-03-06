#include <iostream>
#include <cmath>

// ЗАДАЧА: посчитать число пи с какой-то точностью


double f(double x){
    return sqrt(4.0 - x*x);
}

double calc(size_t N){
    double S = 0;

    double Si = 0;
    double xi = 0;
    double xj = 0;

    double h = 2.0 / N;
    
    for (size_t i = 0; i < N; ++i){
        xi = h * i;
        xj = h * (i + 1);
        Si = ((f(xi) + f(xj)) / 2) * h;  //xj = x_{i+1}
        S += Si;
    }
    return S;
}


int main(){
    
    size_t N;
    std::cin >> N;

    double result;
    result = calc(N);

    std::cerr << "result = ";
    printf("%.9lf\n", result);

    return 0;
}