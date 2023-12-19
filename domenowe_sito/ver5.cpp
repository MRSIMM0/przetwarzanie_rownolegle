#include <iostream>
#include <vector>
#include <cmath>
#include <omp.h>
#include <algorithm> // Do sortowania

void printPrimes(const std::vector<int>& primes) {
    for (size_t i = 0; i < primes.size(); ++i) {
        if (i > 0 && i % 10 == 0) {
            std::cout << std::endl;
        }
        std::cout << primes[i] << " ";
    }
    std::cout << "\nZnaleziono " << primes.size() << " liczb pierwszych." << std::endl;
}

void sieveOfEratosthenes(int M, int N, std::vector<int>& primes) {
    std::vector<bool> isPrime(N + 1, true);
    isPrime[0] = isPrime[1] = false;
    int limit = std::sqrt(N);

    // Skreślanie wielokrotności liczb pierwszych do sqrt(N)
    for (int p = 2; p <= limit; ++p) {
        if (isPrime[p]) {
            for (int i = p * p; i <= N; i += p) {
                isPrime[i] = false;
            }
        }
    }

    // Zbieranie liczb pierwszych w równoległy sposób
    #pragma omp parallel
    {
        std::vector<int> localPrimes;
        #pragma omp for nowait
        for (int i = M; i <= N; i++) {
            if (isPrime[i]) {
                localPrimes.push_back(i);
            }
        }

        // Zabezpieczenie dostępu do współdzielonego zasobu
        #pragma omp critical
        {
            primes.insert(primes.end(), localPrimes.begin(), localPrimes.end());
        }
    }
    
    // Sortowanie wyników
    std::sort(primes.begin(), primes.end());
}

int main() {
    int M = 2;
    int N = 100;
    std::vector<int> primes;

    sieveOfEratosthenes(M, N, primes);
    printPrimes(primes);

    return 0;
}
