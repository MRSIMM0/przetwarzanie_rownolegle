#include <iostream>
#include <cmath>
#include <time.h>
#include <vector>
#include <cmath>
#include <omp.h>

bool isPrime(int num) {
    if (num <= 1) return false;
    if (num == 2) return true;
    if (num % 2 == 0) return false;
    
    int limit = std::sqrt(num);
    for (int i = 3; i <= limit; i += 2) {
        if (num % i == 0) return false;
        
    }
    return true;
}

///ROWNOLEGLE WSZYTKIE PO KOLEI DOMENOWE do end
#define ROWS 100
#define NUM_THREADS 4
#define CACHE_LINE_SIZE 16
#define SIZE (NUM_THREADS * CACHE_LINE_SIZE * ROWS)


void all_domain_all(int start ,int end) {
    int tablica[SIZE];
    int local_counts[NUM_THREADS] = {0};

    #pragma omp parallel num_threads(NUM_THREADS)
    {
        int thread_id = omp_get_thread_num();
        int count = 0;
        #pragma omp for schedule(static)
        for (int num = start; num <= end ; ++num) {
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

///ROWNOLEGLE WSZYTKIE PO KOLEI DOMENOWE do sqrt(end)
#define ROWS 100
#define NUM_THREADS 4
#define CACHE_LINE_SIZE 16
#define SIZE (NUM_THREADS * CACHE_LINE_SIZE * ROWS)


void all_domain_sqrt(int start ,int end) {
    int tablica[SIZE];
    int local_counts[NUM_THREADS] = {0};

    #pragma omp parallel num_threads(NUM_THREADS)
    {
        int thread_id = omp_get_thread_num();
        int count = 0;
        int limit = sqrt(end);
        #pragma omp for schedule(static)
        for (int num = start; num <= limit ; ++num) {
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



template<typename Func, typename... Args>
void measureExecutionTime(std::string message,Func func, Args... args) {
    clock_t spstart, spstop;
    spstart = clock();
    func(args...);
    spstop = clock();
    double time = (message, (double)(spstop - spstart)/CLOCKS_PER_SEC);
    std::cout<<message<<" "<<time<<" sekund"<<std::endl;
}


int main(int argc, char *argv[]) {

    int start = 2;
    int end = 20000000;


    if(argc == 3) {
        start = atoi(argv[1]);
        end = atoi(argv[2]);
    }

    measureExecutionTime("Po kolei all", all_domain_all ,start, end);
    measureExecutionTime("Po kolei sqrt", all_domain_sqrt ,start, end);
    return 0;
}
