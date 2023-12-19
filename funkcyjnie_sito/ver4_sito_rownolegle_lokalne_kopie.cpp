#include <iostream>
#include <vector>
#include <cmath>
#include <omp.h>

void sieve_of_eratosthenes_parallel(int M, int N) {
    if (N < 2 || M > N) {
        std::cout << "Niepoprawny zakres." << std::endl;
        return;
    }

    int sqrtN = static_cast<int>(sqrt(N));
    std::vector<bool> prime(N + 1, true);
    prime[0] = prime[1] = false;

    std::vector<int> small_primes;
    for (int i = 2; i <= sqrtN; ++i) {
        if (prime[i]) {
            small_primes.push_back(i);
            for (int j = i * i; j <= sqrtN; j += i) {
                prime[j] = false;
            }
        }
    }

    omp_set_num_threads(4); // Ustawienie liczby wątków

    #pragma omp parallel
    {
        std::vector<bool> private_prime(N + 1, true);
        #pragma omp for schedule(static)
        for (int i = 0; i < small_primes.size(); ++i) {
            int p = small_primes[i];
            for (int multiple = std::max(p * p, (M + p - 1) / p * p); multiple <= N; multiple += p) {
                private_prime[multiple] = false;
            }
        }

        #pragma omp critical
        {
            for (int i = M; i <= N; ++i) {
                if (!private_prime[i]) {
                    prime[i] = false;
                }
            }
        }
    }

    // int count = 0;
    // for (int p = M; p <= N; p++) {
    //     if (prime[p]) {
    //         std::cout << p << " ";
    //         if (++count % 10 == 0) {
    //             std::cout << std::endl;
    //         }
    //     }
    // }

    // std::cout << std::endl << "Znaleziono " << count << " liczb pierwszych w zakresie od " << M << " do " << N << "." << std::endl;
}

template<typename Func, typename... Args>
void measureExecutionTime(std::string message, Func func, Args... args) {
    clock_t start, stop;
    start = clock();
    func(args...);
    stop = clock();
    double time = (double)(stop - start)/CLOCKS_PER_SEC;
    std::cout << message << " trwało " << time << " sekund" << std::endl;
}

int main() {
    int M = 2;
    int N = 20000000;
    measureExecutionTime("Sito Eratostenesa", sieve_of_eratosthenes_parallel, M, N);

    return 0;
}
