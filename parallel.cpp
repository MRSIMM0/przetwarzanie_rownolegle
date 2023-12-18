#include <iostream>
#include <cmath>
#include <omp.h>

///ROWNOLEGLE WSZYTKIE PO KOLEI DOMENOWE
#define ROWS 100
#define NUM_THREADS 4
#define CACHE_LINE_SIZE 16  // Assuming 64-byte cache line size and 4 bytes per int
#define SIZE (NUM_THREADS * CACHE_LINE_SIZE * ROWS)

int tablica[SIZE];

void all_domain(int limit) {
    int local_counts[NUM_THREADS] = {0};

    #pragma omp parallel num_threads(NUM_THREADS)
    {
        int thread_id = omp_get_thread_num();
        int count = 0;

        // Experiment with different scheduling strategies
        #pragma omp for schedule(static)
        for (int num = 2; num <= limit; ++num) {
            if (isPrime(num)) {
                if (count < ROWS) {
                    int index = thread_id * CACHE_LINE_SIZE * ROWS + count;
                    tablica[index] = num;
                    count++;
                }
            }
        }

        local_counts[thread_id] = count;
    }

}
////////////////////
