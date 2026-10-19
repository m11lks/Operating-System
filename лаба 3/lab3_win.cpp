#include <stdio.h>
#include <windows.h>
#include <chrono>
#include <atomic>
#include <iomanip>
#include <iostream>

using namespace std;

const long long N = 100000000;
const int block_size = 4316020;

std::atomic<int> next_block(0);
double total_sum[64];
HANDLE doneEvents[64];

DWORD WINAPI ThreadFunc(LPVOID lpParam) {
    int id = (int)(long long)lpParam;
    int all_blocks = (N + block_size - 1) / block_size;
    
    while (true) {
        int current_pos = next_block.fetch_add(1);

        if (current_pos >= all_blocks) {
                SetEvent(doneEvents[id]); 
                break;
        }

        int start = current_pos * block_size;
        int end = (start + block_size > N) ? N : start + block_size;
        double first_sum = 0.0;

        for (int i = start; i < end; i++) {
            double x = (i + 0.5) / (double)N;
            first_sum += 4.0 / (1.0 + x * x);
        }
        total_sum[id] += first_sum; 
        SetEvent(doneEvents[id]); 
        SuspendThread(GetCurrentThread()); 
    }
    return 0;
}

int main() {
    int threads_to_test[] = { 1, 2, 4, 8, 12, 16 };
    
    printf("| Threads |      Pi Result      | Time (ms) | Speed (M.iter/sec) |\n");
    printf("---------------------------------------------------------------------\n");

    for (int t = 0; t < 6; t++) {
        int num_threads = threads_to_test[t];
        next_block = 0;
        
        for (int i = 0; i < 64; i++) {
            total_sum[i] = 0.0;
            doneEvents[i] = CreateEvent(NULL, FALSE, FALSE, NULL); 
        }

        HANDLE* threads = new HANDLE[num_threads];
        auto start_time = std::chrono::high_resolution_clock::now();

        for (int i = 0; i < num_threads; i++) {
            threads[i] = CreateThread(NULL, 0, ThreadFunc, (LPVOID)(long long)i, 0, NULL);
        }

        int all_blocks = (N + block_size - 1) / block_size;

        while (true) {
            WaitForMultipleObjects(num_threads, doneEvents, TRUE, INFINITE);
            if (next_block >= all_blocks) break;
            for (int i = 0; i < num_threads; i++) {
                ResumeThread(threads[i]);
            }
        }
        auto end_time = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> final_time = end_time - start_time;

        double pi = 0.0;
        for (int i = 0; i < num_threads; i++) {
            pi += total_sum[i];
            TerminateThread(threads[i], 0); 
            CloseHandle(threads[i]);
            CloseHandle(doneEvents[i]);
        }
        pi /= N;

        double time_ms = final_time.count() * 1000;
        double speed = (N / final_time.count()) / 1000000.0;

        printf("| %-7d | %-18.10f | %-9.2f | %-18.2f |\n", num_threads, pi, time_ms, speed);
        delete[] threads;
    }
    return 0;
}