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

std::vector<long long> find_base_primes(long long sqrtN) {
    std::vector<bool> prime_sqrtN(sqrtN + 1, true);
    std::vector<long long> base_primes;

    for (long long p = 2; p <= sqrtN; ++p) {
        if (prime_sqrtN[p]) {
            base_primes.push_back(p);
            for (long long i = p * p; i <= sqrtN; i += p) {
                prime_sqrtN[i] = false;
            }
        }
    }

    return base_primes;
}

std::vector<long long> sieve_of_eratosthenes(long long M, long long N) {
    long long sqrtN = static_cast<long long>(std::sqrt(N));
    std::vector<long long> base_primes;
    std::vector<bool> is_prime(N - M + 1, true);
    std::vector<long long> primes;

    #pragma omp parallel
    {
        #pragma omp single
        {
            base_primes = find_base_primes(sqrtN);
        }

        #pragma omp for schedule(dynamic)
        for (size_t i = 0; i < base_primes.size(); ++i) {
            long long p = base_primes[i];
            for (long long j = std::max(p*p, (M + p - 1) / p * p); j <= N; j += p) {
                is_prime[j - M] = false;
            }
        }

        std::vector<long long> local_primes;
        #pragma omp for schedule(dynamic)
        for (long long i = M; i <= N; ++i) {
            if (is_prime[i - M]) {
                bool isPrime = true;
                for (long long p : base_primes) {
                    if (p * p > i) break;
                    if (i % p == 0) {
                        isPrime = false;
                        break;
                    }
                }
                if (isPrime) local_primes.push_back(i);
            }
        }

        #pragma omp critical
        primes.insert(primes.end(), local_primes.begin(), local_primes.end());
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
    // printPrimes(sieve_of_eratosthenes(M, N));
    measureExecutionTime(sieve_of_eratosthenes, M, N);
    return 0;
}
