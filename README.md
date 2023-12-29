<div style="font-size: 20px;" align="center">
    <div style="font-weight: 500; font-size: 2em; line-height: 50px;"><bold>Sprawozdanie z Projektu i Eksperymentu Obliczeniowego</bold></div>
    <br/>
    <br/>
    <br/>
    <p style="font-weight: 500;">Laboratorium z Przetwarzania Równoległego</p>
    <br/>
    <br/>
    <br/>
    <br/>
    <br/>
    <br/>
    Termin oddania: 30.12.2023
    <br/>
    <br/>
    <br/>
     Pierwszy termin oddania: 30.12.2023
    <br/>
    <br/>
    <br/>
    <br/>
    Wersja I
    <br/>
    <br/>
    <br/>
    <br/>
    Autorzy:
    <br/>
    Szymon Pasieczny

</div>
<div style="page-break-after: always;"></div>

## 1. Wstęp
Celem niniejszego projektu jest analiza i ocena efektywności przetwarzania równoległego w systemie komputerowym z procesorem wielordzeniowym. Projekt koncentruje się na badaniu wydajności obliczeniowej przy wykorzystaniu pamięci współdzielonej w kontekście zastosowania algorytmów równoległych do rozwiązywania problemu znajdowania liczb pierwszych w określonym przedziale. Kluczowym aspektem jest eksploracja różnych strategii zrównoleglenia, w tym podziału domenowego i funkcyjnego, oraz równoważenie obciążeń procesorów przy użyciu odpowiednio skonfigurowanych wątków OpenMP.

## 1.2 Opis Wykorzystanego Systemu Obliczeniowego: MacBook Air M2

1. **Procesor**:
   - **Oznaczenie**: Apple M2
   - **Liczba procesorów fizycznych**: 1 - integrowany chip Apple M2
   - **Liczba procesorów logicznych**: 8 rdzeni CPU (4 rdzenie wydajnościowe, 4 rdzenie efektywności)
   - **Liczba rdzeni GPU**: 10 rdzeni GPU

2. **Pamięć podręczna procesora**:
   - **Pamięć L1 i L2**: Zintegrowana, specyfikacja nieopisana przez Apple
   - **Pamięć L3**: Zintegrowana, współdzielona, specyfikacja nieopisana przez Apple

3. **System operacyjny**:
   - **Wersja**: MacOs Ventura 13.3

4. **Oprogramowanie do kodowania i testów**:
   - **Nazwa**: Visual Studio Code

5. **Dodatkowe specyfikacje**:
   - **Pamięć RAM**: 8Gb

## 1.2 Znaczenie i cel eksperymentu
Projekt ma na celu nie tylko praktyczne zastosowanie teoretycznej wiedzy zdobytej na zajęciach, ale również stanowi okazję do eksploracji nowych technologii i platform sprzętowych. Analiza przeprowadzona na Macbooku Air z procesorem M2 pozwoli na zgłębienie wiedzy na temat możliwości i ograniczeń nowoczesnych technologii Apple w kontekście przetwarzania równoległego.
<div style="page-break-after: always;"></div>

## 2. Prezentacja Wariantów Kodów
### **Wariant 1 - Podejście klasyczne do wyznacznia liczb pierwszych [SKWLP]**
   ```cpp
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
   ```
   Ten fragment kodu nie wykonywał przetwarzania równoległego. Wykonywał się sekwencyjnie i służył do weryfikacji wyników i obliczania przyspieszenia.
### **Wariant 2 - Sito Erastotelesa [SSE]**
  ```cpp
std::vector<long long> sieveOfEratosthenes(long long lower, long long upper) {
    const long long segmentSize = 1000000;
    std::vector<long long> primes;
    long long sqrtUpper = std::sqrt(upper);

    std::vector<bool> prime(sqrtUpper + 1, true);
    for (long long p = 2; p * p <= sqrtUpper; p++) {
        if (prime[p]) {
            for (long long i = p * p; i <= sqrtUpper; i += p)
                prime[i] = false;
        }
    }

      for (long long low = lower; low <= upper; low += segmentSize) {
          std::vector<bool> segmentPrime(segmentSize, true);
          long long high = std::min(low + segmentSize - 1, upper);

          for (long long p = 2; p * p <= high; p++) {
              if (prime[p]) {
                  for (long long i = std::max(p * p, (low + p - 1) / p * p); i <= high; i += p)
                      segmentPrime[i - low] = false;
              }
          }

          for (long long i = low; i <= high; i++) {
              if (segmentPrime[i - low]) {
                  primes.push_back(i);
              }
          }
      }

      return primes;
  }
  ```
  Ten fragment kodu również nie wykorzystuje elementów przetwarzania równoległego ,i służy nam za punkt odniesienia w wyznaczaniu przyspieszenia. Algorytm ten to segmentowe sito Erastotelesa, został wykorzystany ze wzgledu na mniejsze wykorzystanie pamięci.
### **Wariant 3 - Rozproszona wersja podejścia klasycznego [RKWLP]**
```cpp
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

std::vector<int> printPrimes(int start, int end) {
    std::vector<int> primes;
    #pragma omp parallel for schedule(dynamic) shared(primes)
    for (int i = start; i <= end; i++) {
        if (isPrime(i)) {
            #pragma omp critical
            {
                primes.push_back(i);
            }
        }
    }
    return primes;
}
```
- **Opis**:
  - Jest to zrównoleglona wersja algorytmu z  Wariantu 1. Wszystkie wątki zapisują wyniki swoich obliczeń w jednym wektorze, co wymaga synchronizacji przy użyciu `#pragma omp critical`. Funkcja ta wykorzystuje podejście domenowe - dane dzielone są między wątki, z których każdy wykonuje swoją część.
- **Podział pracy:** Każda liczba w zakresie od `start` do `end` jest oddzielnym zadaniem.

- **Sposób przydziału:** Dynamiczny przydział zadań za pomocą OpenMP.

- **Wybór podziału i przydziału:** Dynamiczny przydział jest odpowiedni ze względu na zróżnicowany czas obliczeń dla różnych liczb.

- **Dyrektywy i klauzule OpenMP:** `#pragma omp parallel for schedule(dynamic)` dla równoległości, `#pragma omp critical` dla synchronizacji.

- **Potencjalne problemy poprawnościowe:** Użycie `#pragma omp critical` minimalizuje ryzyko warunków wyścigowych.

- **Potencjalne problemy efektywnościowe:**
  - **False sharing:** Minimalne, dzięki ostrożnemu zarządzaniu dostępem do `primes`.
  - **Synchronizacja:** `critical` wprowadza narzut, ale jest niezbędna dla poprawności danych.

### **Wariant 4 -  Rozproszona wersja podejścia klasycznego - lokalne kopie [URKWLP]:**
```cpp
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

std::vector<int> printPrimes(int start, int end) {
    std::vector<int> primes;

    #pragma omp parallel
    {
        std::vector<int> local_primes;
        #pragma omp for schedule(static)
        for (int i = start; i <= end; i++) {
            if (isPrime(i)) {
                local_primes.push_back(i);
            }
        }

        #pragma omp critical
        {
            primes.insert(primes.end(), local_primes.begin(), local_primes.end());
        }
    }
    return primes;
}
```
- **Opis**:
  - Jest to ulepszona wersja Wariantu 3. Każdy wątek operuje na lokalnej wersji tablicy, a pózniej wyniki przetwarzania każdego wątku połączone są w sekcji omp critical. Funkcja ta wykorzystuje podejście domenowe - dane dzielone są między wątki, z których każdy wykonuje swoją część.

- **Podział pracy:** Zadania są podzielone na pojedyncze liczby w zakresie od `start` do `end`, gdzie każda liczba jest sprawdzana oddzielnie.

- **Sposób przydziału:** Zadania są przydzielane statycznie (`schedule(static)`), co oznacza, że każdy wątek otrzymuje zbliżoną liczbę zadań do wykonania.

- **Wybór podziału i przydziału:** Statyczny przydział jest odpowiedni w przypadkach, gdy każde zadanie ma podobny czas wykonania, co jest prawdopodobne w tym scenariuszu.

- **Dyrektywy i klauzule OpenMP:** Użyto `#pragma omp parallel` do utworzenia regionu równoległego i `#pragma omp for schedule(static)` do równoległego iterowania. `#pragma omp critical` służy do bezpiecznego scalania wyników.

- **Potencjalne problemy poprawnościowe:**
  - **Wyścig:** Zabezpieczony przez `#pragma omp critical`, który zapewnia bezpieczny dostęp do wspólnego zasobu.

- **Potencjalne problemy efektywnościowe:**
  - **False sharing:** Zminimalizowany dzięki lokalnym wektorom w każdym wątku.
  - **Synchronizacja:** Jest minimalna, ograniczona do momentu scalania wyników, co zmniejsza narzut czasowy.


### **Wariant 5 - Domenowa wersja rozproszonego sita Erastotelesa [RDSE]:**
```cpp
  std::vector<long long> sieveOfEratosthenes(long long M, long long N) {
      std::vector<long long> primes;
      std::vector<bool> isPrime(N + 1, true);
      isPrime[0] = isPrime[1] = false;
      long long limit = std::sqrt(N);

      #pragma omp parallel for schedule(dynamic)
      for (long long p = M; p <= limit; ++p) {
          if (isPrime[p]) {
              for (long long i = p * p; i <= N; i += p) {
                  isPrime[i] = false;
              }
          }
      }

      for (long long i = M; i <= N; ++i) {
          if (isPrime[i]) {
            primes.push_back(i);
          }
      }
      return primes;
  }
```
- **Opis**:
  - Jest to pierwsza próba zrównoleglenia sita eratostenesa (Wariant 2). Funkcja ta wykorzystuje podejście domenowe - dane dzielone są między wątki, z których każdy wykonuje swoją część.
- **Podział Pracy**
  - **Wielkość Zbioru Zadań**: Zbiór zadań obejmuje sprawdzanie każdej liczby od 2 do `N` w celu oznaczenia jej jako pierwszą lub nie.
  - **Sposób Przydziału Zadań do Procesów**: Zadania są przydzielane dynamicznie do dostępnych wątków. Każdy wątek bierze na siebie część zakresu do sprawdzenia, co pozwala na lepsze wykorzystanie zasobów obliczeniowych.

- **Dyrektywy OpenMP**
  - **Wykorzystane Dyrektywy**: `#pragma omp parallel for schedule(dynamic)` jest użyta do równoległego iterowania przez liczby od `M` do pierwiastka z `N`.
  - **Znaczenie Dyrektyw**: Ta dyrektywa pozwala na równoległe przetwarzanie pętli, co znacznie przyspiesza proces identyfikacji liczb pierwszych.

- **Problemy Poprawnościowe**
  - **Wyścigi**: Istnieje potencjalny problem wyścigu związany z aktualizacją wektora `isPrime` w wewnętrznej pętli. Różne wątki mogą próbować jednocześnie zaktualizować ten sam indeks wektora, co może prowadzić do niespójności danych. Rozwiązaniem tego problemu może być użycie atomowych operacji lub struktur synchronizacji.

- **Problemy Efektywnościowe**
  - **False Sharing**: Może występować, gdy wątki modyfikują elementy wektora `isPrime`, które znajdują się w tej samej linii pamięci podręcznej. Ten problem może wpływać na wydajność przez nieefektywne wykorzystanie pamięci podręcznej i zwiększenie opóźnień.
  - **Synchronizacja**: Narzut związany z synchronizacją jest ograniczony dzięki zastosowaniu `schedule(dynamic)` w OpenMP, ale false sharing może nadal wpływać na ogólną wydajność.

### **Wariant 6 - Rozproszone segmentowe sito Erastotelesa [URDSSE]**
```cpp
std::vector<long long> sieveOfEratosthenes(long long lower, long long upper) {
    const long long segmentSize = 1000000;
    std::vector<long long> primes;
    long long sqrtUpper = std::sqrt(upper);

    if(lower == 0){
        lower=2;
    }

    std::vector<bool> prime(sqrtUpper + 1, true);
    for (long long p = 2; p * p <= sqrtUpper; p++) {
        if (prime[p]) {
            for (long long i = p * p; i <= sqrtUpper; i += p)
                prime[i] = false;
        }
    }

    #pragma omp parallel
    {
        std::vector<bool> segmentPrime(segmentSize);
        std::vector<long long> localPrimes;

        #pragma omp for schedule(dynamic)
        for (long long low = lower; low <= upper; low += segmentSize) {
            std::fill(segmentPrime.begin(), segmentPrime.end(), true);
            long long high = std::min(low + segmentSize - 1, upper);

            for (long long p = 2; p * p <= high; p++) {
                if (prime[p]) {
                    for (long long i = std::max(p * p, (low + p - 1) / p * p); i <= high; i += p)
                        segmentPrime[i - low] = false;
                }
            }

            for (long long i = low; i <= high; i++) {
                if (segmentPrime[i - low]) {
                    localPrimes.push_back(i);
                }
            }
        }

        #pragma omp critical
        {
            primes.insert(primes.end(), localPrimes.begin(), localPrimes.end());
        }
    }

    return primes;
}
```
- **Opis:** Jest to zupełnie nowe podejście. Zamiast podstawowego algorytmu wykorzystany został algorytm segmentowy, co pozwala na efektywne pamięciowo przeszukiwanie o wiele większych zbiorów liczb.
- **Podział pracy:** Przetwarzanie jest podzielone na segmenty liczbowe, z których każdy jest oddzielnie przetwarzany przez wątki.

- **Sposób przydziału:** Praca jest przydzielana w ramach bloków segmentowych, co pozwala na równomierne rozłożenie obciążenia między wątki.

- **Wybór podziału i przydziału:** Podział na segmenty pozwala na efektywniejsze zarządzanie pamięcią i obciążeniem procesorów, szczególnie przy dużych zakresach liczb.

- **Dyrektywy i klauzule OpenMP:** Użyto `#pragma omp parallel` do tworzenia równoległych bloków przetwarzania, co zwiększa efektywność obliczeń.

- **Potencjalne problemy poprawnościowe:**
  - **Wyścig:** Zminimalizowany dzięki niezależności przetwarzania w poszczególnych segmentach.

- **Potencjalne problemy efektywnościowe:**
  - **False sharing:** Ograniczony dzięki lokalnym strukturom danych w każdym wątku.
  - **Synchronizacja:** Zredukowana do minimum, ponieważ większość operacji jest wykonywana niezależnie przez wątki.

### Wariant 7 - Zmodyfikowana Wersja Wariantu 6 - Testy innych strategii [GRDSSE]
```cpp
std::vector<long long> sieveOfEratosthenes(long long lower, long long upper) {
    const long long segmentSize = 1000000;
    std::vector<long long> primes;

    long long limit = std::sqrt(std::sqrt(upper) + 1);
    std::vector<bool> prime(limit, true);

    if(lower == 0){
        lower = 2;
    }

    #pragma omp parallel
    {
        std::vector<bool> segmentPrime(segmentSize);
        std::vector<long long> localPrimes;

        #pragma omp for schedule(guided)
        for (long long low = lower; low <= upper; low += segmentSize) {
            std::fill(segmentPrime.begin(), segmentPrime.end(), true);
            long long high = std::min(low + segmentSize - 1, upper);

            for (long long p = 2; p * p <= high; p++) {
                if (prime[p]) {
                    for (long long i = std::max(p * p, (low + p - 1) / p * p); i <= high; i += p)
                        segmentPrime[i - low] = false;
                }
            }

            for (long long i = low; i <= high; i++) {
                if (segmentPrime[i - low]) {
                    localPrimes.push_back(i);
                }
            }
        }

        #pragma omp critical
        {
            primes.insert(primes.end(), localPrimes.begin(), localPrimes.end());
        }
    }

    return primes;
}
```


- **Opis:** Przedstawniony fragment jest zmodyfikowaną wersją Waraintu 6. Został zmieniony sposó przydzialu iteracji pętli - z `dynamic` na `guided`. W tym fragmęcie sprawdziliśmy czy zmienna `limit` czyli zakres poczatkowego sprawdzenia liczb pierwszych ma wpływ na poprawność i szybkość działania agordytmu.
- **Podział pracy:** Przetwarzanie jest podzielone na segmenty liczbowe, z których każdy jest oddzielnie przetwarzany przez wątki.

- **Sposób przydziału:** Praca jest przydzielana w ramach bloków segmentowych, co pozwala na równomierne rozłożenie obciążenia między wątki.

- **Wybór podziału i przydziału:** Podział na segmenty pozwala na efektywniejsze zarządzanie pamięcią i obciążeniem procesorów, szczególnie przy dużych zakresach liczb.

- **Dyrektywy i klauzule OpenMP:** Użyto `#pragma omp parallel` do tworzenia równoległych bloków przetwarzania, co zwiększa efektywność obliczeń.

- **Potencjalne problemy poprawnościowe:**
  - **Wyścig:** Zminimalizowany dzięki niezależności przetwarzania w poszczególnych segmentach.

- **Potencjalne problemy efektywnościowe:**
  - **False sharing:** Ograniczony dzięki lokalnym strukturom danych w każdym wątku.
  - **Synchronizacja:** Zredukowana do minimum, ponieważ większość operacji jest wykonywana niezależnie przez wątki.

### Wariant 8 - Funkcyjna wersja rozproszonego sita Erastotelesa[NFRSE]:
```cpp
std::vector<long long> sieve_of_eratosthenes(long long M, long long N) {
    std::vector<bool> is_prime(N + 1, true);
    std::vector<long long> primes;

    long long sqrtN = static_cast<long long>(std::sqrt(N)) + 1;

    #pragma omp parallel
    {
        #pragma omp for schedule(static)
        for (long long p = 2; p <= sqrtN; ++p) {
            if (is_prime[p]) {
                for (long long i = p * p; i <= N; i += p) {
                    is_prime[i] = false;
                }
            }
        }

        #pragma omp for schedule(dynamic)
        for (long long i = std::max(M, 2LL); i <= N; i++) {
            if (is_prime[i]) {
                #pragma omp critical
                primes.push_back(i);
            }
        }
    }

    return primes;
}
```
- **Opis:** Przedstawiony kod jest pierwsza próbą implementacji podejścia funkcyjnego.

- **Podział pracy:** Przetwarzanie jest podzielone na liczby od `M` do `N`, gdzie każda liczba jest oddzielnie analizowana pod kątem bycia liczbą pierwszą.

- **Sposób przydziału:** Przydział zadań odbywa się dynamicznie (`schedule(dynamic)`), umożliwiając elastyczne zarządzanie obciążeniem wątków.

- **Wybór podziału i przydziału:** Dynamiczny przydział jest odpowiedni dla różnorodnych czasów wykonania zadań, co pomaga w zrównoważeniu obciążenia procesorów.

- **Dyrektywy i klauzule OpenMP:** Wykorzystano `#pragma omp parallel` wraz z `#pragma omp for schedule(dynamic)` do efektywnego równoległego przetwarzania.

- **Potencjalne problemy poprawnościowe:**
  - **Wyścig:** Zminimalizowany dzięki odpowiedniej synchronizacji i niezależności zadań.

- **Potencjalne problemy efektywnościowe:**
  - **False sharing:** Zminimalizowany dzięki niezależnym operacjom w ramach wątków.
  - **Synchronizacja:** Ograniczona dzięki dynamicznemu przydziałowi zadań.
### Wariant 9 - Funkcyjna wersja rozproszonego sita Erastotelesa z dyrektywą single[UFRSE]
```cpp
std::vector<long long> sieve_of_eratosthenes(long long M, long long N) {
    long long sqrtN = static_cast<long long>(std::sqrt(N));
    std::vector<bool> prime_sqrtN(sqrtN + 1, true);
    std::vector<bool> is_prime(N - M + 1, true);
    std::vector<long long> base_primes;

    // Wspólny blok równoległy dla obu faz
    #pragma omp parallel
    {
        // Faza 1: Wyznaczanie liczb pierwszych do pierwiastka kwadratowego z N
        #pragma omp single
        {
            for (long long p = 2; p <= sqrtN; ++p) {
                if (prime_sqrtN[p]) {
                    for (long long i = p * p; i <= sqrtN; i += p) {
                        prime_sqrtN[i] = false;
                    }
                }
            }
            for (long long p = 2; p <= sqrtN; ++p) {
                if (prime_sqrtN[p]) {
                    base_primes.push_back(p);
                }
            }
        }

        // Faza 2: Równoległe wykreślanie wielokrotności liczb pierwszych w zakresie M do N
        #pragma omp for nowait
        for (long long i = M; i <= N; ++i) {
            for (long long p : base_primes) {
                if (i % p == 0 && i != p) {
                    #pragma omp critical
                    is_prime[i - M] = false;
                    break;
                }
            }
        }
    }

    std::vector<long long> primes;
    for (long long i = M; i <= N; ++i) {
        if (is_prime[i - M]) {
            primes.push_back(i);
        }
    }

    return primes;
}
```
- **Opis:** W tym Wariancie zostało zastosowane podejście, które bardziej przypomina podejście funkcyjne. Przetwarzanie jest podzielone na dwie fazy - szukanie liczb pierwszych do `sqrt(n)` i ich wykreślanie.
- **Podział pracy:** Przetwarzanie jest podzielone na dwie fazy, co pozwala na lepsze wykorzystanie zasobów obliczeniowych i efektywniejsze zarządzanie pamięcią.

- **Sposób przydziału:** Przydział zadań odbywa się dynamicznie w ramach każdej z faz, wykorzystując dyrektywy OpenMP do efektywnego zarządzania przetwarzaniem.

- **Dyrektywy i klauzule OpenMP:**
  - Użyto `#pragma omp parallel` dla tworzenia równoległego regionu przetwarzania.
  - `#pragma omp single` wykorzystano do wykonania inicjalizacji tylko przez jeden wątek, co zapobiega redundancji i zwiększa wydajność.

- **Potencjalne problemy poprawnościowe:**
  - **Wyścig:** Zminimalizowany dzięki starannie zaprojektowanemu podziałowi pracy i zastosowaniu dyrektyw OpenMP.

- **Potencjalne problemy efektywnościowe:**
  - **False sharing:** Ograniczony przez użycie lokalnych struktur danych.
  - **Synchronizacja:** Zredukowana do minimum dzięki optymalizacji podziału pracy i zastosowaniu odpowiednich dyrektyw OpenMP.
### Wariant 10 - Funkcyjna wersja rozproszonego sita Erastotelesa z dyrektywa single i lokalną kopią primes[UFRSEL]
```cpp
std::vector<long long> find_base_primes(long long sqrtN) {
    std::vector<bool> prime_sqrtN(sqrtN + 1, true);
    std::vector<long long> base_primes;

    for (long long p = 2; p <= sqrtN; ++p) {
        if (prime_sqrtN[p]) {
            base_primes.push_back(p);
            for (long long i = p * p; i <= sqrtN; i += p) {
                prime_sqrtN[i] = false;
            }
        }
    }

    return base_primes;
}

std::vector<long long> sieve_of_eratosthenes(long long M, long long N) {
    long long sqrtN = static_cast<long long>(std::sqrt(N));
    std::vector<long long> base_primes;
    std::vector<bool> is_prime(N - M + 1, true);
    std::vector<long long> primes;

    #pragma omp parallel
    {
        #pragma omp single
        {
            base_primes = find_base_primes(sqrtN);
        }

        #pragma omp for schedule(dynamic)
        for (size_t i = 0; i < base_primes.size(); ++i) {
            long long p = base_primes[i];
            for (long long j = std::max(p*p, (M + p - 1) / p * p); j <= N; j += p) {
                is_prime[j - M] = false;
            }
        }

        std::vector<long long> local_primes;
        #pragma omp for schedule(dynamic)
        for (long long i = M; i <= N; ++i) {
            if (is_prime[i - M]) {
                bool isPrime = true;
                for (long long p : base_primes) {
                    if (p * p > i) break;
                    if (i % p == 0) {
                        isPrime = false;
                        break;
                    }
                }
                if (isPrime) local_primes.push_back(i);
            }
        }

        #pragma omp critical
        primes.insert(primes.end(), local_primes.begin(), local_primes.end());
    }

    return primes;
}
```
- **Opis:**  W tej wersji zaimplementowano sito Eratostenesa z optymalizacjami, skupiając się na efektywności obliczeniowej. Funkcja `find_base_primes` jest nowością, identyfikuje liczby pierwsze do pierwiastka z `N`, co jest kluczowe dla efektywności pamięciowej i przetwarzania. Główna funkcja `sieve_of_eratosthenes` wykorzystuje te liczby do eliminacji wielokrotności w zakresie od `M` do `N`.

- **Podział pracy:** Przetwarzanie odbywa się w dwóch etapach: identyfikacja bazowych liczb pierwszych i ich wykorzystanie w głównej fazie sita.

- **Sposób przydziału:** Dynamiczny przydział zadań z użyciem OpenMP, dostosowujący się do zmiennego obciążenia w różnych fazach algorytmu.

- **Dyrektywy i klauzule OpenMP:**
  - Wykorzystano `#pragma omp parallel` dla zrównoleglenia pewnych segmentów kodu, zwiększając efektywność obliczeniową.

- **Potencjalne problemy poprawnościowe:**
  - **Wyścig:** Minimalizowany przez zastosowanie dyrektyw OpenMP i niezależność operacji w różnych segmentach kodu.

- **Potencjalne problemy efektywnościowe:**
  - **False sharing:** Ograniczony dzięki efektywnemu zarządzaniu pamięcią i niezależnym operacjom w wątkach.
  - **Synchronizacja:** Zredukowana przez dynamiczne rozdzielanie zadań i skuteczne wykorzystanie wątków.

<div style="page-break-after: always;"></div>

## 3.Prezentacja Wyników i Omówienie Eksperymentu
 ### **3.1 Testowane Wersje Kodów:**
Testowane były Warianty:

Tabele wyników przetwarzania sekwencyjnego:
<div style="display:flex; justify-content: center; gap: 50px" align="center">
  <table border="1">
    <tr>
      <th>Zakres</th>
      <th>Czas [sek]</th>
      <th>Prędkość</th>
    </tr>
    <tr>
      <th>Dla sekwencyjnego</th>
      <th></th>
      <th></th>
    </tr>
    <tr>
      <td>0 - 150 000 000</td>
      <td>24.4643</td>
      <td>6 131 000</td>
    </tr>
    <tr>
      <td>150 000 000 - 300 000 000</td>
      <td>41.6647</td>
      <td>3 600 000</td>
    </tr>
    <tr>
      <td>0 - 300 000 000</td>
      <td>65.5019</td>
      <td>4 579 000</td>
    </tr>
  </table>
    <table border="1">
    <tr>
      <th>Zakres</th>
      <th>Czas [sek]</th>
      <th>Prędkość</th>
    </tr>
    <tr>
      <th>Dla domenowego</th>
      <th></th>
      <th></th>
    </tr>
    <tr>
      <td>0 - 6 000 000 000</td>
      <td>28.8243</td>
      <td>208 158 000</td>
    </tr>
    <tr>
      <td>6 000 000 000 - 12 000 000 000</td>
      <td>28.7015</td>
      <td>209 048 000</td>
    </tr>
    <tr>
      <td>0 - 12 000 000 000</td>
      <td>59.6637</td>
      <td>201 127 000</td>
    </tr>
    <tr>
      <th>Dla funkcyjnego</th>
      <th></th>
      <th></th>
    </tr>
    <tr>
      <td>0 - 250 000 000</td>
      <td>1.13552</td>
      <td>220 163 000</td>
    </tr>
    <tr>
      <td>250 000 000 - 500 000 000</td>
      <td>1.16579</td>
      <td>214 447 000</td>
    </tr>
    <tr>
      <td>0 - 500 000 000</td>
      <td>2.28449</td>
      <td>218 867 000</td>
    </tr>
  </table>
</div>

**Wyniki przetwarzania równoległego:**

Przysp. - przyśpieszenie programu względem wersji sekwencyjnej

Prędkość - ilość przetworzonych liczb na sekundę obliczeń

Efekt. - efektywność, iloraz przyśpieszenia i ilości wykorzystywanych wątków

<div align="center">
<table border="1">
  <tr>
    <th></th>
    <th colspan="4">RKWLP (8 wątków)</th>
    <th colspan="4">RKWLP (4 wątki)</th>
  </tr>
  <tr>
    <th>Zakres</th>
    <th>Czas[sek]</th>
    <th>Przysp.</th>
    <th>Prędkość</th>
    <th>Efekt.</th>
    <th>Czas[sek]</th>
    <th>Przysp.</th>
    <th>Prędkość</th>
    <th>Efekt.</th>
  </tr>
  <tr>
    <th>0 - 150 000 000</th>
    <th>7.7995</th>
    <th>3.1366</th>
    <th>19 232 000</th>
    <th>0.3921</th>
    <th>7.3346</th>
    <th>3.3355</th>
    <th>20 451 000</th>
    <th>0.8339</th>
  </tr>
  <tr>
    <th>150 000 000 - 300 000 000</th>
    <th>11.3807</th>
    <th>3.6610</th>
    <th>13 180 000</th>
    <th>0.4576</th>
    <th>11.7500</th>
    <th>3.5459</th>
    <th>12 766 000</th>
    <th>0.8865</th>
  </tr>
  <tr>
    <th>0 - 300 000 000</th>
    <th>19.1810</th>
    <th>3.4149</th>
    <th>15 640 000</th>
    <th>0.4269</th>
    <th>19.0162</th>
    <th>3.4445</th>
    <th>15 776 000</th>
    <th>0.8611</th>
  </tr>
</table>
</div>
<div align="center">
<table border="1">
  <tr>
    <th></th>
    <th colspan="4">URKWLP (8 wątków)</th>
    <th colspan="4">URKWLP (4 wątki)</th>
  </tr>
  <tr>
    <th>Zakres</th>
    <th>Czas[sek]</th>
    <th>Przysp.</th>
    <th>Prędkość</th>
    <th>Efekt.</th>
    <th>Czas[sek]</th>
    <th>Przysp.</th>
    <th>Prędkość</th>
    <th>Efekt.</th>
  </tr>
  <tr>
    <th>0 - 150 000 000</th>
    <th>7.0258</th>
    <th>3.4821</th>
    <th>21 350 000</th>
    <th>0.4353</th>
    <th>9.7849</th>
    <th>2.5002</th>
    <th>15 330 000</th>
    <th>0.6251</th>
  </tr>
  <tr>
    <th>150 000 000 - 300 000 000</th>
    <th>11.1582</th>
    <th>3.7340</th>
    <th>13 443 000</th>
    <th>0.4668</th>
    <th>13.7676</th>
    <th>3.0263</th>
    <th>10 895 000</th>
    <th>0.7566</th>
  </tr>
  <tr>
    <th>0 - 300 000 000</th>
    <th>19.1174</th>
    <th>3.4263</th>
    <th>15 693 000</th>
    <th>0.4283</th>
    <th>26.4474</th>
    <th>2.4767</th>
    <th>11 343 000</th>
    <th>0.6192</th>
  </tr>
</table>
</div>
<div align="center">
<table border="1">
  <tr>
    <th></th>
    <th colspan="4">URDSSE (8 wątków)</th>
    <th colspan="4">URDSSE (4 wątki)</th>
  </tr>
  <tr>
    <th>Zakres</th>
    <th>Czas[sek]</th>
    <th>Przysp.</th>
    <th>Prędkość</th>
    <th>Efekt.</th>
    <th>Czas[sek]</th>
    <th>Przysp.</th>
    <th>Prędkość</th>
    <th>Efekt.</th>
  </tr>
  <tr>
    <th>0 - 6 000 000 000</th>
    <th>24.5834</th>
    <th>1.1725</th>
    <th>244 067 000</th>
    <th>0.1466</th>
    <th>25.8005</th>
    <th>0.8951</th>
    <th>232 558 000</th>
    <th>0.2238</th>
  </tr>
  <tr>
    <th>6 000 000 000 - 12 000 000 000</th>
    <th>-</th>
    <th>-</th>
    <th>-</th>
    <th>-</th>
    <th>-</th>
    <th>-</th>
    <th>-</th>
    <th>-</th>
  </tr>
  <tr>
    <th>0 - 12 000 000 000</th>
    <th>51.7358</th>
    <th>1.1532</th>
    <th>231 948 000</th>
    <th>0.1442</th>
    <th>56.1390</th>
    <th>0.9409</th>
    <th>213 755 000</th>
    <th>0.2352</th>
  </tr>
</table>
</div>
<div align="center">
<table border="1">
  <tr>
    <th></th>
    <th colspan="4">GRDSSE (8 wątków)</th>
    <th colspan="4">GRDSSE (4 wątki)</th>
  </tr>
  <tr>
    <th>Zakres</th>
    <th>Czas[sek]</th>
    <th>Przysp.</th>
    <th>Prędkość</th>
    <th>Efekt.</th>
    <th>Czas[sek]</th>
    <th>Przysp.</th>
    <th>Prędkość</th>
    <th>Efekt.</th>
  </tr>
  <tr>
    <th>0 - 6 000 000 000</th>
    <th>5.9555</th>
    <th>4.8399</th>
    <th>1 007 472 000</th>
    <th>0.6050</th>
    <th>9.1387</th>
    <th>3.1541</th>
    <th>656 548 000</th>
    <th>0.7885</th>
  </tr>
  <tr>
    <th>6 000 000 000 - 12 000 000 000</th>
    <th>7.0492</th>
    <th>4.0716</th>
    <th>851 184 000</th>
    <th>0.5090</th>
    <th>9.1943</th>
    <th>3.1217</th>
    <th>652 578 000</th>
    <th>0.7804</th>
  </tr>
  <tr>
    <th>0 - 12 000 000 000</th>
    <th>13.8583</th>
    <th>4.3053</th>
    <th>865 926 000</th>
    <th>0.5382</th>
    <th>19.3009</th>
    <th>3.0912</th>
    <th>621 762 000</th>
    <th>0.7728</th>
  </tr>
</table>
</div>
<div align="center">
<table border="1">
  <tr>
    <th></th>
    <th colspan="4">UFRSE (8 wątków)</th>
    <th colspan="4">UFRSE (4 wątki)</th>
  </tr>
  <tr>
    <th>Zakres</th>
    <th>Czas[sek]</th>
    <th>Przysp.</th>
    <th>Prędkość</th>
    <th>Efekt.</th>
    <th>Czas[sek]</th>
    <th>Przysp.</th>
    <th>Prędkość</th>
    <th>Efekt.</th>
  </tr>
  <tr>
    <th>0 - 250 000 000</th>
    <th>15.8288</th>
    <th>0.0717</th>
    <th>15 795 000</th>
    <th>0.0090</th>
    <th>15.4083</th>
    <th>0.0737</th>
    <th>16 225 000</th>
    <th>0.0184</th>
  </tr>
  <tr>
    <th>250 000 000 - 500 000 000</th>
    <th>16.6369</th>
    <th>0.0701</th>
    <th>15 027 000</th>
    <th>0.0088</th>
    <th>15.5489</th>
    <th>0.0750</th>
    <th>16 078 000</th>
    <th>0.0188</th>
  </tr>
  <tr>
    <th>0 - 500 000 000</th>
    <th>32.3571</th>
    <th>0.0706</th>
    <th>15 453 000</th>
    <th>0.0088</th>
    <th>31.1471</th>
    <th>0.0733</th>
    <th>16 053 000</th>
    <th>0.0183</th>
  </tr>
</table>
</div>
<div align="center">
<table border="1">
  <tr>
    <th></th>
    <th colspan="4">UFRSEL (8 wątków)</th>
    <th colspan="4">UFRSEL (4 wątki)</th>
  </tr>
  <tr>
    <th>Zakres</th>
    <th>Czas[sek]</th>
    <th>Przysp.</th>
    <th>Prędkość</th>
    <th>Efekt.</th>
    <th>Czas[sek]</th>
    <th>Przysp.</th>
    <th>Prędkość</th>
    <th>Efekt.</th>
  </tr>
  <tr>
    <th>0 - 250 000 000</th>
    <th>6.5524</th>
    <th>0.1733</th>
    <th>38 154 000</th>
    <th>0.0217</th>
    <th>4.2485</th>
    <th>0.2673</th>
    <th>59 851 000</th>
    <th>0.0668</th>
  </tr>
  <tr>
    <th>250 000 000 - 500 000 000</th>
    <th>6.1931</th>
    <th>0.1882</th>
    <th>40 368 000</th>
    <th>0.0235</th>
    <th>5.9534</th>
    <th>0.1958</th>
    <th>41 996 000</th>
    <th>0.0490</th>
  </tr>
  <tr>
    <th>0 - 500 000 000</th>
    <th>12.7608</th>
    <th>0.1790</th>
    <th>39 182 000</th>
    <th>0.0224</th>
    <th>10.1805</th>
    <th>0.2244</th>
    <th>49 116 000</th>
    <th>0.0561</th>
  </tr>
</table>
</div>


c) **Omówienie Wyników:**


**Porównanie Jakości Rozwiązań:**[Prędkość przetwarzania vs. czas obliczeń].
**Analiza Efektywności Zrównoleglenia:** [Przyspieszenie, Efektywność, Prędkość].
## Punkt 4: Wnioski

**Podejście klasyczne**

Zrównoleglenie podejścia klasycznego dało pozytywne rezultaty - czas wykonywania algorytmu RKWLP był w każdym pomiarze ponad 3-krotnie krótszy od sekwencyjnej wersji. Co ciekawe, dwukrotne zwiększenie liczby zaangażowanych w obliczenia wątków z czterech do ośmiu praktycznie nie wpłynęło na efektywność algorytmu. Powodów takiego zachowania systemu może być kilka, np.:

1. Niewystarczające obciążenie pracy: Algorytm generowania liczb pierwszych może być stosunkowo prosty, a zakres danych może być niewystarczająco duży, aby uzyskać pełne korzyści z 8 wątków. W takim przypadku narzut związany z tworzeniem i zarządzaniem wątkami może przeważać nad korzyściami z równoległego przetwarzania.

2. Przeskok pomiędzy danymi dla poszczególnych wątków: W przypadku równoległego przetwarzania, kiedy każdy wątek otrzymuje fragment danych do przetworzenia, ważne jest, aby te fragmenty były równo podzielone i nie nakładały się na siebie. Jeżeli zachodzi przeplatanie się danych, wątki mogą konkurować o dostęp do współdzielonych zasobów, co może prowadzić do konfliktów i utraty korzyści z równoległego przetwarzania.

3. Słaba skalowalność algorytmu: Nie wszystkie algorytmy dobrze skalują się zwiększając liczbę wątków. Skalowalność zależy od natury algorytmu, jego złożoności obliczeniowej, struktury danych i sposobu dostępu do pamięci. Jeżeli algorytm nie jest dobrze dostosowany do równoległego przetwarzania, dodanie więcej wątków może nie przynieść oczekiwanej poprawy wydajności.

4. Overhead związany z tworzeniem i zarządzaniem wątkami: Tworzenie i zarządzanie wątkami wprowadza pewien narzut czasowy. Jeśli czas potrzebny na ten cel jest porównywalny lub przeważa nad czasem potrzebnym na rzeczywiste obliczenia wątków, to dodanie kolejnych wątków może nie przynieść znaczącej poprawy wydajności.

5. Częste wykonywanie klauzuli `critical`: dla każdej liczby pierwszej w zakresie program musi wkroczyć w sekcję krytyczną, co znacznie spowalnia wykonywanie.

Problem opisany w ostatnim podpunkcie rozwiązaliśmy w kolejnej badanej wersji algorytmu - URKWLP - za pomocą lokalnych kopii tablicy `primes` (`local_primes`). Dzięki temu poszczególne wątki wchodziły do sekcji krytycznej tylko raz, na końcu przetwarzania. Dzięki temu mogliśmy odnotować ciekawą obserwację - prędkość ulepszonego algorytmu w porównaniu z pierwowzorem (RKWLP) wzrosła, gdy ten używał 8 wątków, ale malała przy używaniu 4 wątków. Ze względu na mniej synchronizacji w czasie wykonywania algorymu, ten jest bardziej zależny od liczby wykorzystywanych wątków.

**domenowe sito**

Pierwsze poprawne podejście do zrównoleglenia sita Erastotelesa nie dało satysfakcjonujących efektów - algorytm URDSSE, czyli rozproszone segmentowe sito Erastotelesa zmniejszył czas przetwarzania tylko o ok. 15-18% względem sekwencyjnego sita przy 8 używanych wątkach. Używając 4 wątków algorytm nawet zwolnił (przyśpieszenie było mniejsze od 1). Głównym powodem tak słabych wyników mógł być nieodpowiednio dobrany sposób przydzielania wątkom iteracji pętli - `schedule(dynamic)`. Wniosek ten wysnuliśmy po przetestowaniu innego sposobu - `schedule(guided)`. Ta mała zmiana w kodzie pozwoliła na przyśpieszenie kodu około czterokrotnie. Prawdopodobnie powód jest taki, że przydzielanie `guided` dobrze się sprawuje, gdy poszczególne iteracje różnią się długością przetwarzania. W tym algorytmie występuje takie zjawisko, ponieważ w pierwszych segmentach jest znacznie więcej liczb pierwszych niż w zakresach z większymi liczbami. 

**funkcyjne sito**

**Porównanie Podejść:** [Wariant 1 vs. Wariant 2, ...].
**Podsumowanie Zrównoważenia Przetwarzania:** [Analiza zrównoważenia pracy procesorów].
**Ocena Efektywności Skalowania:** [Efektywność w zależności od liczby procesorów].
**Ograniczenia Efektywnościowe:** [Dominujące ograniczenia w kodzie].
(Używanie miar względnych: np. "czas przetwarzania 2 razy krócej" zamiast "o 2 sekundy krócej").
