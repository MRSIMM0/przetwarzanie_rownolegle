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

## 1.2 Znaczenie i cel ekspreymentu
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
  Ten fragment kodu również nie wykorzystuje elementów przetwarzania równoległego ,i służy nam za punkt odniesienia w wyznaczaniu przyspieszenia. Algorytm ten to segemntowe sito Erastotelesa, został wykorzystany ze wzgledu na mniejsze wykorzystanie pamięci.
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
  - Jest to zrównoleglona wersja algorytmu z  Wariantu 1. Wszystkie wątki zapisują wyniki swoich obliczeń w jednym wektorze, co wymaga synchronizacji przy użyciu #pragma omp critical.Funkcja ta wykorzystuje podejście domenowe - dane dzielone są między wątki, z których każdy wykonuje swoją część.
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
  - Jest to ulepszona wersja Wraiantu 3. Każdy wątek operuje na lokalnej wersji tablicy, a pozniej wyniki przetwarzania każdego wątku połączone są w sekcji omp critical.Funkcja ta wykorzystuje podejście domenowe - dane dzielone są między wątki, z których każdy wykonuje swoją część.

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
  - Jest to pierwsza próba zrównoleglenia sita eratostenesa (Wariant 2).Funkcja ta wykorzystuje podejście domenowe - dane dzielone są między wątki, z których każ**dy wykonuje swoją część.
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
- **Opis:** Jest to zupełnie nowe podejście. Zamiast podstawowego algorutmu wykorzystany został algorym segemntowy co pozwala na efektywne pamięciowo przeszukiwanie o wiele większych zbiorów liczb.
- **Podział pracy:** Przetwarzanie jest podzielone na segmenty liczbowe, z których każdy jest oddzielnie przetwarzany przez wątki.

- **Sposób przydziału:** Praca jest przydzielana w ramach bloków segmentowych, co pozwala na równomierne rozłożenie obciążenia między wątki.

- **Wybór podziału i przydziału:** Podział na segmenty pozwala na efektywniejsze zarządzanie pamięcią i obciążeniem procesorów, szczególnie przy dużych zakresach liczb.

- **Dyrektywy i klauzule OpenMP:** Użyto `#pragma omp parallel` do tworzenia równoległych bloków przetwarzania, co zwiększa efektywność obliczeń.

- **Potencjalne problemy poprawnościowe:**
  - **Wyścig:** Zminimalizowany dzięki niezależności przetwarzania w poszczególnych segmentach.

- **Potencjalne problemy efektywnościowe:**
  - **False sharing:** Ograniczony dzięki lokalnym strukturom danych w każdym wątku.
  - **Synchronizacja:** Zredukowana do minimum, ponieważ większość operacji jest wykonywana niezależnie przez wątki.

### Wariant 7 - Zmodyfikowana Wersja Wariantu 6 - lokalne kopie primes [GRDSSE]
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


- **Opis:** Przedstawniony fragment jest zmodyfikowaną wersją Waraintu 6. Został zmieniony sposó przydzialu iteracji pętli - z dynamic na guided.
- **Podział pracy:** Przetwarzanie jest podzielone na segmenty liczbowe, z których każdy jest oddzielnie przetwarzany przez wątki.

- **Sposób przydziału:** Praca jest przydzielana w ramach bloków segmentowych, co pozwala na równomierne rozłożenie obciążenia między wątki.

- **Wybór podziału i przydziału:** Podział na segmenty pozwala na efektywniejsze zarządzanie pamięcią i obciążeniem procesorów, szczególnie przy dużych zakresach liczb.

- **Dyrektywy i klauzule OpenMP:** Użyto `#pragma omp parallel` do tworzenia równoległych bloków przetwarzania, co zwiększa efektywność obliczeń.

- **Potencjalne problemy poprawnościowe:**
  - **Wyścig:** Zminimalizowany dzięki niezależności przetwarzania w poszczególnych segmentach.

- **Potencjalne problemy efektywnościowe:**
  - **False sharing:** Ograniczony dzięki lokalnym strukturom danych w każdym wątku.
  - **Synchronizacja:** Zredukowana do minimum, ponieważ większość operacji jest wykonywana niezależnie przez wątki.

### Wariant 8 - Funkcujna wersja rozproszonego sita Erastotelesa[NFRSE]:
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
- **Opis:** Przedsawiony kod jest pierwsza póbą implementacji podejścia funkcyjnego.

- **Podział pracy:** Przetwarzanie jest podzielone na liczby od `M` do `N`, gdzie każda liczba jest oddzielnie analizowana pod kątem bycia liczbą pierwszą.

- **Sposób przydziału:** Przydział zadań odbywa się dynamicznie (`schedule(dynamic)`), umożliwiając elastyczne zarządzanie obciążeniem wątków.

- **Wybór podziału i przydziału:** Dynamiczny przydział jest odpowiedni dla różnorodnych czasów wykonania zadań, co pomaga w zrównoważeniu obciążenia procesorów.

- **Dyrektywy i klauzule OpenMP:** Wykorzystano `#pragma omp parallel` wraz z `#pragma omp for schedule(dynamic)` do efektywnego równoległego przetwarzania.

- **Potencjalne problemy poprawnościowe:**
  - **Wyścig:** Zminimalizowany dzięki odpowiedniej synchronizacji i niezależności zadań.

- **Potencjalne problemy efektywnościowe:**
  - **False sharing:** Zminimalizowany dzięki niezależnym operacjom w ramach wątków.
  - **Synchronizacja:** Ograniczona dzięki dynamicznemu przydziałowi zadań.
### Wariant 9 - Funkcujna wersja rozproszonego sita Erastotelesa z dyrektywa single[UFRSE]
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
- **Opis:** W tym Wariancie zostało zastosowane podejście któro bardziej przypomina podejście funkcuje. Przetwarzanie jest podzelone na dwie fazy - szukanie liczb pierwszych do sqrt(n) i ich wykreślanie.
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
### Wariant 10 - Funkcujna wersja rozproszonego sita Erastotelesa z dyrektywa single i lokalną kopią primes[UFRSEL]
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
    </tr>
    <tr>
      <th>Dla sekwencyjnego</th>
      <th>Sekwencyjne Klasycznie</th>
    </tr>
    <tr>
      <td>0 - 150 000 000</td>
      <td>24.4643</td>
    </tr>
    <tr>
      <td>150 000 000 - 300 000 000</td>
      <td>41.6647</td>
    </tr>
    <tr>
      <td>0 - 300 000 000</td>
      <td>65.5019</td>
    </tr>
  </table>
    <table border="1">
    <tr>
      <th>Zakres</th>
      <th>Czas [sek]</th>
    </tr>
    <tr>
      <th>Dla domenowego</th>
      <th>Sekwencyjnie Sito</th>
    </tr>
    <tr>
      <td>0 - 6 000 000 000</td>
      <td>28.8243</td>
    </tr>
    <tr>
      <td>6 000 000 000 - 12 000 000 000</td>
      <td>28.7015</td>
    </tr>
    <tr>
      <td>0 - 12 000 000 000</td>
      <td>59.6637</td>
    </tr>
    <tr>
      <th>Dla funkcyjnego</th>
      <th>Sekwencyjnie Sito</th>
    </tr>
    <tr>
      <td>0 - 250 000 000</td>
      <td>1.13552</td>
    </tr>
    <tr>
      <td>250 000 000 - 500 000 000</td>
      <td>1.16579</td>
    </tr>
    <tr>
      <td>0 - 500 000 000</td>
      <td>2.28449</td>
    </tr>
  </table>
</div>

Wyniki przetwarzania równoległego:

<div align="center">
  <table border="1">
    <tr>
      <th></th>
      <th colspan="2">RKWLP (8 wątków)</th>
      <th></th>
      <th colspan="2">RKWLP (4 wątki)</th>
    </tr>
    <tr>
      <th>Zakres</th>
      <th>Czas[sek]</th>
      <th>Przyspieszenie względem programu sekwencyjnego</th>
      <th>Czas[sek]</th>
      <th>Przyspieszenie względem programu sekwencyjnego</th>
    </tr>
    <tr>
      <th>0 - 150 000 000</th>
      <th>7.79952</th>
      <th>3.136641742</th>
      <th>7.33463</th>
      <th>3.335451141</th>
    </tr>
    <tr>
      <th>150 000 000 - 300 000 000</th>
      <th>11.3807</th>
      <th>3.660996248</th>
      <th>11.75</th>
      <th>3.545931915</th>
    </tr>
    <tr>
      <th>0 - 300 000 000</th>
      <th>19.181</th>
      <th>3.414936656</th>
      <th>19.0162</th>
      <th>3.444531505</th>
    </tr>
  </table>
</div>
<div align="center">
  <table border="1">
    <tr>
      <th></th>
      <th colspan="2">URKWLP (8 wątków)</th>
      <th></th>
      <th colspan="2">URKWLP (4 wątki)</th>
    </tr>
    <tr>
      <th>Zakres</th>
      <th>Czas[sek]</th>
      <th>Przyspieszenie względem programu sekwencyjnego</th>
      <th>Czas[sek]</th>
      <th>Przyspieszenie względem programu sekwencyjnego</th>
    </tr>
    <tr>
      <th>0 - 150 000 000</th>
      <th>7.02582</th>
      <th>3.482056187</th>
      <th>9.78488</th>
      <th>2.500214617</th>
    </tr>
    <tr>
      <th>150 000 000 - 300 000 000</th>
      <th>11.1582</th>
      <th>3.733998315</th>
      <th>13.7676</th>
      <th>3.026286353</th>
    </tr>
    <tr>
      <th>0 - 300 000 000</th>
      <th>19.1174</th>
      <th>3.426297509</th>
      <th>26.4474</th>
      <th>2.476685799</th>
    </tr>
  </table>
</div>
<div align="center">
  <table border="1">
    <tr>
      <th></th>
      <th colspan="2">URDSSE (8 wątków)</th>
      <th></th>
      <th colspan="2">URDSSE (4 wątków)</th>
    </tr>
    <tr>
      <th>Zakres</th>
      <th>Czas[sek]</th>
      <th>Przyspieszenie względem programu sekwencyjnego</th>
      <th>Czas[sek]</th>
      <th>Przyspieszenie względem programu sekwencyjnego</th>
    </tr>
    <tr>
      <th>0 - 6 000 000 000</th>
      <th>24.5834</th>
      <th>1.172510719</th>
      <th>25.8005</th>
      <th>0.8950954577</th>
    </tr>
    <tr>
      <th>6 000 000 000 - 12 000 000 000</th>
      <th>-</th>
      <th>-</th>
      <th>-</th>
      <th>-</th>
    </tr>
    <tr>
      <th>0 - 12 000 000 000</th>
      <th>51.7358</th>
      <th>1.153238183</th>
      <th>56.139</th>
      <th>0.9409238783</th>
    </tr>
  </table>
</div>
<div align="center">
  <table border="1">
    <tr>
      <th></th>
      <th colspan="2">GRDSSE (8 wątków)</th>
      <th></th>
      <th colspan="2">GRDSSE (4 wątków)</th>
    </tr>
    <tr>
      <th>Zakres</th>
      <th>Czas[sek]</th>
      <th>Przyspieszenie względem programu sekwencyjnego</th>
      <th>Czas[sek]</th>
      <th>Przyspieszenie względem programu sekwencyjnego</th>
    </tr>
    <tr>
      <th>0 - 6 000 000 000</th>
      <th>5.95552</th>
      <th>4.839930015</th>
      <th>9.13869</th>
      <th>3.15409539</th>
    </tr>
    <tr>
      <th>6 000 000 000 - 12 000 000 000</th>
      <th>7.04915</th>
      <th>4.071625657</th>
      <th>9.19431</th>
      <th>3.121658939</th>
    </tr>
    <tr>
      <th>0 - 12 000 000 000</th>
      <th>13.8583</th>
      <th>4.305268323</th>
      <th>19.3009</th>
      <th>3.091239269</th>
    </tr>
  </table>
</div>
<div align="center">
  <table border="1">
    <tr>
      <th></th>
      <th colspan="2">UFRSE (8 wątków)</th>
      <th></th>
      <th colspan="2">UFRSE (4 wątków)</th>
    </tr>
    <tr>
      <th>Zakres</th>
      <th>Czas[sek]</th>
      <th>Przyspieszenie względem programu sekwencyjnego</th>
      <th>Czas[sek]</th>
      <th>Przyspieszenie względem programu sekwencyjnego</th>
    </tr>
    <tr>
      <th>0 - 250 000 000</th>
      <th>15.8288</th>
      <th>0.07173759224</th>
      <th>15.4083</th>
      <th>0.07369534601</th>
    </tr>
    <tr>
      <th>250 000 000 - 500 000 000</th>
      <th>16.6369</th>
      <th>0.07007254957</th>
      <th>15.5489</th>
      <th>0.07497572176</th>
    </tr>
    <tr>
      <th>0 - 500 000 000</th>
      <th>32.3571</th>
      <th>0.07060243347</th>
      <th>31.1471</th>
      <th>0.07334519105</th>
    </tr>
  </table>
</div>
<div align="center">
  <table border="1">
    <tr>
      <th></th>
      <th colspan="2">UFRSEL (8 wątków)</th>
      <th></th>
      <th colspan="2">UFRSEL (4 wątków)</th>
    </tr>
    <tr>
      <th>Zakres</th>
      <th>Czas[sek]</th>
      <th>Przyspieszenie względem programu sekwencyjnego</th>
      <th>Czas[sek]</th>
      <th>Przyspieszenie względem programu sekwencyjnego</th>
    </tr>
    <tr>
      <th>0 - 250 000 000</th>
      <th>6.55244</th>
      <th>0.1732972755</th>
      <th>4.24846</th>
      <th>0.2672780254</th>
    </tr>
    <tr>
      <th>250 000 000 - 500 000 000</th>
      <th>6.1931</th>
      <th>0.1882401382</th>
      <th>5.95336</th>
      <th>0.1958205114</th>
    </tr>
    <tr>
      <th>0 - 500 000 000</th>
      <th>12.7608</th>
      <th>0.1790240424</th>
      <th>10.1805</th>
      <th>0.2243986052</th>
    </tr>
  </table>
</div>


c) **Omówienie Wyników:**


**Porównanie Jakości Rozwiązań:**[Prędkość przetwarzania vs. czas obliczeń].
**Analiza Efektywności Zrównoleglenia:** [Przyspieszenie, Efektywność, Prędkość].
## Punkt 4: Wnioski
**Porównanie Podejść:** [Wariant 1 vs. Wariant 2, ...].
**Podsumowanie Zrównoważenia Przetwarzania:** [Analiza zrównoważenia pracy procesorów].
**Ocena Efektywności Skalowania:** [Efektywność w zależności od liczby procesorów].
**Ograniczenia Efektywnościowe:** [Dominujące ograniczenia w kodzie].
(Używanie miar względnych: np. "czas przetwarzania 2 razy krócej" zamiast "o 2 sekundy krócej").
