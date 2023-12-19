#include <iostream>
#include <vector>
#include <cmath>
#include <omp.h>
/// FALSE SHARING PADDING w sumie to nie dziala - czasy wykonywania sa ok 4 razy dlozsze
const int CACHE_LINE_SIZE = 64; // Rozmiar linii pamięci podręcznej w bajtach
const int PAD = CACHE_LINE_SIZE / sizeof(bool); // Ile booli na linię pamięci

void sieve_of_eratosthenes_parallel(int M, int N) {
    if (N < 2 || M > N) {
        std::cout << "Niepoprawny zakres." << std::endl;
        return;
    }

    // Tablica z paddingiem
    std::vector<bool> prime((N + 1) * PAD, true);
    for (int i = 0; i <= N; ++i) {
        prime[i * PAD] = true;
    }
    prime[0 * PAD] = prime[1 * PAD] = false;

    int sqrtN = static_cast<int>(sqrt(N));
    // Wyznaczanie liczb pierwszych do pierwiastka z N
    std::vector<int> small_primes;
    for (int i = 2; i <= sqrtN; ++i) {
        if (prime[i * PAD]) {
            small_primes.push_back(i);
            for (int j = i * i; j <= sqrtN; j += i) {
                prime[j * PAD] = false;
            }
        }
    }

    omp_set_num_threads(2); // Ustawienie liczby wątków

    // Równoległe oznaczanie wielokrotności jako złożone
    #pragma omp parallel for schedule(static)
    for (int i = 0; i < small_primes.size(); ++i) {
        int p = small_primes[i];
        for (int multiple = std::max(p * p, (M + p - 1) / p * p); multiple <= N; multiple += p) {
            prime[multiple * PAD] = false;
        }
    }

    // int count = 0;
    // for (int p = M; p <= N; p++) {
    //     if (prime[p * PAD]) {
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
