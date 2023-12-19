#include <iostream>
#include <vector>
#include <cmath>
#include <omp.h>

void printPrimes(const std::vector<bool>& isPrime, int M, int N) {
    int count = 0, lineCount = 0;
    for (int i = M; i <= N; ++i) {
        if (isPrime[i]) {
            std::cout << i << " ";
            if (++lineCount % 10 == 0) std::cout << std::endl;
            ++count;
        }
    }
    std::cout << "\nZnaleziono " << count << " liczb pierwszych w zakresie od " << M << " do " << N << "." << std::endl;
}

void sieveOfEratosthenes(int M, int N) {
    std::vector<bool> isPrime(N + 1, true);
    isPrime[0] = isPrime[1] = false;
    int limit = std::sqrt(N);

    #pragma omp parallel
    {
        std::vector<bool> isPrimeLocal = isPrime;

        #pragma omp for schedule(dynamic)
        for (int p = 2; p <= limit; ++p) {
            if (isPrimeLocal[p]) {
                for (int i = p * p; i <= N; i += p) {
                    isPrimeLocal[i] = false;
                }
            }
        }

        #pragma omp critical
        {
            for (int i = 2; i <= N; ++i) {
                if (!isPrimeLocal[i]) {
                    isPrime[i] = false;
                }
            }
        }
    }

    printPrimes(isPrime, M, N);
}

int main() {
    int M, N;

    M = 2;
    N = 40;
    sieveOfEratosthenes(M, N);
    return 0;
}
