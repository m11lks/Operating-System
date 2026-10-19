#include <iostream>
#include <omp.h>
#include <iomanip>
#include <cstdlib>
#include <cstdio>

using namespace std;

int main() {
    const long long N = 100000000;
    const long long block_size = 4316020;
    int threads_to_test[] = { 1, 2, 4, 8, 12, 16 };
    int num_tests = sizeof(threads_to_test) / sizeof(threads_to_test[0]);

    printf("| Threads |       Pi Result      | Time (ms) | Speed (M.iter/sec) |\n");
    printf("|---------|----------------------|-----------|--------------------|\n");

    for (int i = 0; i < num_tests; i++) {
        int num_threads = threads_to_test[i];
        omp_set_num_threads(num_threads);
        double pi = 0.0;
        double start_time = omp_get_wtime();

        #pragma omp parallel for reduction(+:pi) schedule(dynamic, block_size)
        for (long long j = 0; j < N; j++) {
            double x = (j + 0.5) /(double) N;
            pi += 4.0 / (1.0 + x * x);
        }
        pi /= N;
        double end_time = omp_get_wtime();
        
        double total_time_sec = end_time - start_time;
        double total_time_ms = total_time_sec * 1000.0;
        double mips = N / (total_time_sec * 1000000.0);

        printf("| %-7d | %-20.10f | %-9.2f | %-18.2f |\n", num_threads, pi, total_time_ms, mips);
    }
    cout.flush(); 
    _exit(0); 
    return 0;
}