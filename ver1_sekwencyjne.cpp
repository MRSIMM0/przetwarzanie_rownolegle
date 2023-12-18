#include <iostream>
#include <cmath>
#include <time.h>
#include <vector>
#include <cmath>
#include <omp.h>
// SEKEWNCYJNE SITO
std::vector<int> sieveOfEratosthenes(int lower, int upper) {
    std::vector<int> primes;
    std::vector<bool> prime(upper + 1, true);
    prime[0] = prime[1] = false;
    int counter = 0;

    for (int p = 2; p * p <= upper; p++) {
        if (prime[p]) {
            for (int i = std::max(p * p, (lower + p - 1) / p * p); i <= upper; i += p)
                prime[i] = false;
        }
    }

    for (int p = lower; p <= upper; p++) {
        if (prime[p])
            primes.push_back(p);
    }

    return primes;
}
/////////////////////////////////
// SEKWENCYJNE PO KOLEI
bool isPrime(int num) {
    if (num <= 1) return false;
    if (num == 2) return true;
    if (num % 2 == 0) return false;
    
    int limit = std::sqrt(num);
    for (int i = 3; i <= limit; i += 2) {
        if (num % i == 0) return false;
    }
    return true;
}

int countPrimes(int start, int end) {
    std::vector<int> primes;

    for (int i = start; i <= end; i++) {
        if (isPrime(i)) {
            primes.push_back(i);
        }
    }

    return primes.size();
}
/////////////////////////////

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


    if(argc == 3) {
        start = atoi(argv[1]);
        end = atoi(argv[2]);
    }
    std::cout<<countPrimes(start, end)<<std::endl;
    // measureExecutionTime("Po kolei", countPrimes ,start, end);
    measureExecutionTime("Sito", sieveOfEratosthenes, start, end);
    return 0;
}
