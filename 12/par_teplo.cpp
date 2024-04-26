#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <omp.h>
#include <iostream>

int main(int argc, char *argv[]){
    int num_procs = 4;

    int flag = false;

    double *u, *unew, *kmax, maxdelta;
    double eps=1.e-6;
    double h, tau;

    int N;
    int count = 0;

    FILE *ff;

    if(argc !=2 ){
        printf("Usage: exefile npoints\n");
        exit(-1);
    }

    N = atoi(argv[1]);
    if (N == 0) {
        printf("Set N to 1000\n");
        N =1000;
    } else {
        printf("Set N to %d\n", N);
    }

    if((u = (double*) malloc((N+1)*sizeof(double))) == NULL){
        printf("Can't allocate memory for u\n");
        exit(-1);
    }

    if((unew = (double*) malloc((N+1)*sizeof(double))) == NULL){
        printf("Can't allocate memory for unew\n");
        free(u);
        exit(-1);
    }



    if((kmax = (double*) malloc((num_procs)*sizeof(double))) == NULL){
        printf("Can't allocate memory for kmin\n");
        free(u);
        free(unew);
        exit(-1);
    }

    h = 1.0/N;
    tau = 0.5*(h*h);

#pragma omp parallel num_threads(num_procs) private(maxdelta)
    {
    int thread_num = omp_get_thread_num();

    #pragma omp for
        for(int i=1; i<N; ++i){
            u[i] = 0;
        }

    #pragma omp for
        for(int i=0; i<num_procs; ++i){
            kmax[i] = 0;
        }
    
    #pragma omp single
    {
    unew[0] = 1.0;
    u[0] = 1.0;
    unew[N] = 0;
    u[N] = 0;
    }

    while(true){
        #pragma omp for 
        for (int i=1; i<N; ++i){
           unew[i] = u[i]+(tau/(h*h))*(u[i-1]-2*u[i]+u[i+1]);
        }

        maxdelta = 0;

        #pragma omp for
            for (int i=1; i<N; ++i){
                double delta = fabs(unew[i]-u[i]);
                if(delta > maxdelta){
                    maxdelta = delta;
                    kmax[thread_num] = delta;
                }
            }

        #pragma omp single
        {
            for(int i = 0; i < num_procs; ++i) maxdelta = std::max(kmax[i], maxdelta);
            if(maxdelta < eps) flag = true;
            ++count;
        }

        if(flag){break;}

        #pragma omp for
        for(int i = 1; i < N; ++i){
            u[i] = unew[i];
        }
    }
}

    printf("%d steps\n",count);
    if ((ff = fopen("parres", "w+")) == NULL){
       printf("Can't open file\n"); free(u); free(unew); free(kmax);
       exit(-1);
    }

    for(int i=0; i<N+1; ++i)
       fprintf(ff, "%f\n", unew[i]);

    fclose(ff); free(u); free(unew); free(kmax);
    return 0;
}