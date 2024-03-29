#include <iostream>
#include <vector>
#include <cmath>
#include <omp.h>
#include <chrono>

void printPrimes(const std::vector<long long>& primes) {
    for (size_t i = 0; i < primes.size(); ++i) {
        if (i > 0 && i % 10 == 0) {
            std::cout << std::endl;
        }
        std::cout << primes[i] << " ";
    }
    std::cout << "\nZnaleziono " << primes.size() << " liczb pierwszych." << std::endl;
}

std::vector<long long> sieveOfEratosthenes(long long M, long long N) {
    std::vector<long long> primes;
    std::vector<bool> isPrime(N + 1, true);
    isPrime[0] = isPrime[1] = false;
    long long limit = std::sqrt(N);

    #pragma omp parallel for schedule(dynamic)
    for (long long p = M; p <= limit; ++p) {
        if (isPrime[p]) {
            for (long long i = p * p; i <= N; i += p) {
                isPrime[i] = false;
            }
        }
    }

    for (long long i = M; i <= N; ++i) {
        if (isPrime[i]) {
          primes.push_back(i);
        }
    }
    return primes;
}

template<typename Func, typename... Args>
void measureExecutionTime(Func func, Args... args) {
    auto start = std::chrono::high_resolution_clock::now();
    std::vector<long long> result = func(args...);
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

    long long M = std::stoll(argv[1]);
    long long N = std::stoll(argv[2]);

    measureExecutionTime(sieveOfEratosthenes ,M, N);
    return 0;
}