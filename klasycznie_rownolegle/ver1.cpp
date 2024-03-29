#include <iostream>
#include <cmath>
#include <chrono>
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

std::vector<int> printPrimes(int start, int end) {
    std::vector<int> primes;
    int limit = sqrt(end);
    #pragma omp parallel for schedule(dynamic) shared(primes)
    for (int i = start; i <= end; i++) {
        if (isPrime(i)) {
            #pragma omp critical
            {
                primes.push_back(i);
            }
        }
    }

    return primes;
}

template<typename Func, typename... Args>
void measureExecutionTime(Func func, Args... args) {
    auto start = std::chrono::high_resolution_clock::now();
    std::vector<int> result = func(args...);
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    double time = duration.count() / 1e6;
    std::cout << time << std::endl;
    std::cout << result.size() << std::endl;
}


int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cout << "Użycie: ./program M N" << std::endl;
        return 1;
    }

    int M = std::stoi(argv[1]);
    int N = std::stoi(argv[2]);

    measureExecutionTime(printPrimes ,M, N);
    return 0;
}

