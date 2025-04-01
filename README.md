# SO2---projekt
## Opis problemu
Program implementuje klasyczny problem synchronizacji "Dining Philosophers" (Ucztujący Filozofowie), zaproponowany przez Edsgera Dijkstrę.
Problem przedstawia sytuację, w której:

- N filozofów siedzi przy okrągłym stole
- Każdy filozof ma przed sobą talerz
- Między każdymi dwoma talerzami znajduje się jeden widelec (łącznie N widelców)
- Aby zjeść, filozof potrzebuje dwóch widelców (lewego i prawego)
- Każdy filozof na przemian myśli i je

Problem ilustruje wyzwania związane z synchronizacją w systemach współbieżnych, demonstrując potencjalne problemy takie jak:
- **Zakleszczenie (deadlock)** - wszyscy filozofowie jednocześnie podnoszą po jednym widelcu i czekają na drugi
- **Zagłodzenie (starvation)** - niektórzy filozofowie nie mogą uzyskać dostępu do zasobów przez dłuższy czas

## Wątki i ich reprezentacja
Program tworzy N wątków, gdzie każdy wątek reprezentuje jednego filozofa:

Każdy filozof (wątek) wykonuje cykl: myślenie -> próba jedzenia -> jedzenie -> odłożenie widelców
Czasy myślenia i jedzenia są losowe (w zakresie 500-1500 ms)
Każdy filozof wykonuje określoną liczbę cykli (iteracji)

Wątki są tworzone w metodzie start() przy użyciu standardowej biblioteki <thread>:
```cpp
philosophers.emplace_back(&DiningPhilosophers::philosopher, this, i, ref(iterations_count[i]));
```

## Sekcje krytyczne
W programie występują dwie główne sekcje krytyczne:
**Dostęp do widelców** - każdy widelec może być używany tylko przez jednego filozofa naraz
**Dostęp do konsoli** - wyjście z wielu wątków musi być zsynchronizowane, aby uniknąć pomieszania komunikatów

## Rozwiązanie
Spinlock dla widelców:
- Każdy widelec jest chroniony przez obiekt SpinLock
- Implementacja własnej klasy SpinLock z użyciem zmiennych atomowych (std::atomic)


Zapobieganie zakleszczeniu:
- Filozofowie z parzystymi numerami najpierw podnoszą lewy widelec, a potem prawy
- Filozofowie z nieparzystymi numerami najpierw podnoszą prawy widelec, a potem lewy
Ta strategia zapobiega sytuacji, w której wszyscy filozofowie trzymają po jednym widelcu i czekają na drugi


Synchronizacja wyjścia na konsolę:
- Używany jest dodatkowy SpinLock do ochrony dostępu do konsoli
- Metoda log() zapewnia, że tylko jeden wątek naraz może pisać do konsoli

## Dodatkowe informacje
Program używa generatora liczb pseudolosowych Mersenne Twister (std::mt19937) zainicjalizowanego prawdziwie losowym ziarnem
Symulacja kończy się, gdy wszyscy filozofowie wykonają określoną liczbę cykli jedzenia
Każdy widelec jest współdzielonym zasobem między dwóch sąsiadujących filozofów
