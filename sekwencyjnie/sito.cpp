#include <iostream>
#include <cmath>
#include <vector>
#include <cmath>
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

std::vector<long long> sieveOfEratosthenes(long long lower, long long upper) {
    const long long segmentSize = 1000000;
    std::vector<long long> primes;
    long long sqrtUpper = std::sqrt(upper);

    if (lower == 0) {
        lower = 2;
    }

    std::vector<bool> prime(sqrtUpper + 1, true);
    prime[0] = prime[1] = false;
    for (long long p = 2; p * p <= sqrtUpper; p++) {
        if (prime[p]) {
            for (long long i = p * p; i <= sqrtUpper; i += p)
                prime[i] = false;
        }
    }

    for (long long low = lower; low <= upper; low += segmentSize) {
        std::vector<bool> segmentPrime(segmentSize, true);
        long long high = std::min(low + segmentSize - 1, upper);

        for (long long p = 2; p * p <= high; p++) {
            if (prime[p]) {
                for (long long i = std::max(p * p, (low + p - 1) / p * p); i <= high; i += p)
                    segmentPrime[i - low] = false;
            }
        }

        for (long long i = low; i <= high; i++) {
            if (segmentPrime[i - low]) {
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
    // printPrimes(sieveOfEratosthenes(M, N));
    measureExecutionTime(sieveOfEratosthenes, M, N);
    return 0;
}
