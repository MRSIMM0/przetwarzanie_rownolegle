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

std::vector<long long> sieve_of_eratosthenes(long long M, long long N) {
    std::vector<bool> is_prime(N + 1, true);
    std::vector<long long> primes;

    long long sqrtN = static_cast<long long>(std::sqrt(N)) + 1;

    #pragma omp parallel
    {
        #pragma omp for schedule(static)
        for (long long p = 2; p <= sqrtN; ++p) {
            if (is_prime[p]) {
                for (long long i = p * p; i <= N; i += p) {
                    is_prime[i] = false;
                }
            }
        }

        #pragma omp for schedule(dynamic)
        for (long long i = std::max(M, 2LL); i <= N; i++) {
            if (is_prime[i]) {
                #pragma omp critical
                primes.push_back(i);
            }
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
    std::cout << "Czas wykonania: " << time << " sekundy" << std::endl;
    std::cout << "Liczba znalezionych liczb pierwszych: " << result.size() << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cout << "Użycie: ./program M N" << std::endl;
        return 1;
    }

    long long M = std::stoll(argv[1]);
    long long N = std::stoll(argv[2]);
    measureExecutionTime(sieve_of_eratosthenes, M, N);
    // printPrimes(sieve_of_eratosthenes(M, N));

    return 0;
}
