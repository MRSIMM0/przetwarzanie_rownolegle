#include <iostream>
#include <vector>
#include <cmath>
#include <omp.h>
#include <time.h>
#include <algorithm>
#include <numeric>
// zle wyniki
int* sieveOfEratosthenes(int N, int& primeCount) {
    std::vector<bool> prime(N+1, true);
    prime[0] = prime[1] = false;

    int sqrtN = std::sqrt(N);

    #pragma omp parallel
    {
        #pragma omp for
        for (int i = 2; i <= sqrtN; ++i) {
            if (prime[i]) {
                for (int j = i * i; j <= N; j += i) {
                    prime[j] = false;
                }
            }
        }
    }

    primeCount = std::count(prime.begin(), prime.end(), true);
    int* primes = new int[primeCount];
    int index = 0;
    for (int i = 2; i <= N; ++i) {
        if (prime[i]) {
            primes[index++] = i;
        }
    }
    
    return primes;
}

/// sieveOfEratosthenes_func dziala dlugo  zle wyniki
int* markMultiples(int* numbers, int size, int prime) {
    int* result = new int[size];
    #pragma omp parallel for
    for (size_t i = 0; i < size; ++i) {
        result[i] = (numbers[i] % prime == 0 && numbers[i] != prime) ? 0 : numbers[i];
    }
    return result;
}

int* sieveOfEratosthenes_func(int N, int& primeCount) {
    int* numbers = new int[N-1];
    for (int i = 0; i < N-1; ++i) {
        numbers[i] = i + 2;
    }

    int sqrtN = std::sqrt(N);
    for (int i = 0; i < N-1 && numbers[i] <= sqrtN; ++i) {
        if (numbers[i] != 0) {
            int* temp = markMultiples(numbers, N-1, numbers[i]);
            delete[] numbers;
            numbers = temp;
        }
    }

    primeCount = 0;
    for (int i = 0; i < N-1; ++i) {
        if (numbers[i] != 0) {
            primeCount++;
        }
    }

    int* primes = new int[primeCount];
    int j = 0;
    for (int i = 0; i < N-1; ++i) {
        if (numbers[i] != 0) {
            primes[j++] = numbers[i];
        }
    }

    delete[] numbers;
    return primes;
}

///////////// wersja super szybka zle wyniki
void markMultiples(bool* isPrime, int size, int prime) {
    #pragma omp parallel for
    for (int i = prime * prime; i <= size; i += prime) {
        isPrime[i] = false;
    }
}

int* sieveOfEratosthenes_func_v2(int N, int& primeCount) {
    if (N < 2) {
        primeCount = 0;
        return nullptr;
    }

    bool* isPrime = new bool[N + 1];
    std::fill_n(isPrime, N + 1, true);

    int sqrtN = std::sqrt(N);
    for (int i = 2; i <= sqrtN; ++i) {
        if (isPrime[i]) {
            markMultiples(isPrime, N, i);
        }
    }

    primeCount = 0;
    for (int i = 2; i <= N; ++i) {
        if (isPrime[i]) {
            primeCount++;
        }
    }

    int* primes = new int[primeCount];
    int index = 0;
    for (int i = 2; i <= N; ++i) {
        if (isPrime[i]) {
            primes[index++] = i;
        }
    }

    delete[] isPrime;
    return primes;
}




//////
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
    int primeCount;

    if(argc == 3) {
        start = atoi(argv[1]);
        end = atoi(argv[2]);
    }
    measureExecutionTime("Sito", sieveOfEratosthenes_func , end, primeCount);
    std::cout << "Znaleziono " << primeCount << " liczb pierwszych w zakresie od " << start << " do " << end << std::endl;


    measureExecutionTime("Sito", sieveOfEratosthenes , end, primeCount);
    std::cout << "Znaleziono " << primeCount << " liczb pierwszych w zakresie od " << start << " do " << end << std::endl;
    return 0;
}
