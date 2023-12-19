#include <iostream>
#include <vector>
#include <cmath>
#include <omp.h>

void sieve_of_eratosthenes_parallel(int M, int N) {
    if (N < 2 || M > N) {
        std::cout << "Niepoprawny zakres." << std::endl;
        return;
    }

    std::vector<bool> prime(N + 1, true);
    prime[0] = prime[1] = false;

 
        int sqrtN = static_cast<int>(sqrt(N));
        #pragma omp parallel for schedule(dynamic)
        for (int p = 2; p <= sqrtN; p++) {
            if (prime[p]) {
                for (int i = p * p; i <= N; i += p) {
                    #pragma omp critical
                    prime[i] = false;
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

int main() {
    int M = 2;
    int N = 20000000;
    sieve_of_eratosthenes_parallel(M, N);
    
    return 0;
}
