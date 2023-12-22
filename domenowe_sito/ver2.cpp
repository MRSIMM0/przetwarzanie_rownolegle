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

    #pragma omp parallel for
    for (int p = 2; p <= limit; ++p) {
        if (isPrime[p]) {
            for (int i = p * p; i <= N; i += p)
                #pragma omp critical
                isPrime[i] = false;
        }
    }

    printPrimes(isPrime, M, N);
}

template<typename Func, typename... Args>
void measureExecutionTime(std::string message,Func func, Args... args) {
    clock_t spstart, spstop;
    spstart = clock();
    func(args...);
    spstop = clock();
    double time = (message, (double)(spstop - spstart)/CLOCKS_PER_SEC);
    std::cout<<message<<" "<<time<<" sekund"<<std::endl;
}


int main() {
    int M, N;

    M = 2;
    N = 20000000;
    measureExecutionTime("Sito z critical",sieveOfEratosthenes, M, N);
    return 0;
}