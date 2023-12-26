#include <iostream>
#include <vector>
#include <cmath>
#include <omp.h>
#include <chrono>
#include <algorithm>

void printPrimes(const std::vector<int>& primes) {
    for (size_t i = 0; i < primes.size(); ++i) {
        if (i > 0 && i % 10 == 0) {
            std::cout << std::endl;
        }
        std::cout << primes[i] << " ";
    }
    std::cout << "\nZnaleziono " << primes.size() << " liczb pierwszych." << std::endl;
}

std::vector<long long> sieveOfEratosthenes(long long lower, long long upper) {
    const long long segmentSize = 1000000;
    std::vector<long long> primes;
    long long sqrtUpper = std::sqrt(upper);

    if(lower == 0){
        lower=2;
    }

    std::vector<bool> prime(sqrtUpper + 1, true);
    for (long long p = 2; p * p <= sqrtUpper; p++) {
        if (prime[p]) {
            for (long long i = p * p; i <= sqrtUpper; i += p)
                prime[i] = false;
        }
    }

    #pragma omp parallel
    {
        std::vector<bool> segmentPrime(segmentSize);
        std::vector<long long> localPrimes;

        #pragma omp for schedule(dynamic)
        for (long long low = lower; low <= upper; low += segmentSize) {
            std::fill(segmentPrime.begin(), segmentPrime.end(), true);
            long long high = std::min(low + segmentSize - 1, upper);

            for (long long p = 2; p * p <= high; p++) {
                if (prime[p]) {
                    for (long long i = std::max(p * p, (low + p - 1) / p * p); i <= high; i += p)
                        segmentPrime[i - low] = false;
                }
            }

            for (long long i = low; i <= high; i++) {
                if (segmentPrime[i - low]) {
                    localPrimes.push_back(i);
                }
            }
        }

        #pragma omp critical
        {
            primes.insert(primes.end(), localPrimes.begin(), localPrimes.end());
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
