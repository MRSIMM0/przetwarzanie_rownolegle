#include <iostream>
#include <vector>
#include <cmath>
#include <omp.h>
/// NAJLEPSZA FUNKNCJA
void sieve_of_eratosthenes_parallel(int M, int N) {
    if (N < 2 || M > N) {
        std::cout << "Niepoprawny zakres." << std::endl;
        return;
    }

    std::vector<bool> prime(N + 1, true);
    prime[0] = prime[1] = false;

 
        int sqrtN = static_cast<int>(sqrt(N));
    // Wyznaczanie liczb pierwszych do pierwiastka z N
    std::vector<int> small_primes;
    for (int i = 2; i <= sqrtN; ++i) {
        if (prime[i]) {
            small_primes.push_back(i);
            for (int j = i * i; j <= sqrtN; j += i) {
                prime[j] = false;
            }
        }
    }
    omp_set_num_threads(4); // ?????????????????????? przy 2 watkach zwraca dobry wynik (0-4) przy 4 watkach zwraca zle wyniki (0-2)
    // Równoległe oznaczanie wielokrotności jako złożone
    #pragma omp parallel for schedule(dynamic)
    for (int i = 0; i < small_primes.size(); ++i) {
        int p = small_primes[i];

        for (int multiple = std::max(p * p, (M + p - 1) / p * p); multiple <= N; multiple += p) {
            prime[multiple] = false;
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
void measureExecutionTime(std::string message,Func func, Args... args) {
    clock_t spstart, spstop;
    spstart = clock();
    func(args...);
    spstop = clock();
    double time = (message, (double)(spstop - spstart)/CLOCKS_PER_SEC);
    std::cout<<message<<" "<<time<<" sekund"<<std::endl;
}

int main() {
    int M = 2;
    int N = 20000000;
    measureExecutionTime("Sito ",sieve_of_eratosthenes_parallel, M, N);

    return 0;
}
