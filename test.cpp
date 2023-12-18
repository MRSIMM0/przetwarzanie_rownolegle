#include <iostream>
#include <vector>
#include <cmath>

int countPrimes(int N) {
    if (N < 2) return 0;

    std::vector<bool> isPrime(N + 1, true);
    isPrime[0] = isPrime[1] = false;

    int sqrtN = std::sqrt(N);
    for (int i = 2; i <= sqrtN; ++i) {
        if (isPrime[i]) {
            for (int j = i * i; j <= N; j += i) {
                isPrime[j] = false;
            }
        }
    }

    int primeCount = 0;
    for (int i = 2; i <= N; ++i) {
        if (isPrime[i]) {
            primeCount++;
        }
    }

    return primeCount;
}

int main() {
    int N = 20000000;
    std::cout << "Number of primes between 2 and " << N << ": " << countPrimes(N) << std::endl;
    return 0;
}
