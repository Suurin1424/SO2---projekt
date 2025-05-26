# Wielowątkowy serwer czatu
## Opis problemu
Serwer czatu to aplikacja umożliwiająca komunikację wielu klientów poprzez rozsyłanie wiadomości do wszystkich uczestników. Głównym wyzwaniem jest synchronizacja dostępu do współdzielonych zasobów (listy klientów) oraz efektywne zarządzanie wątkami. Należy unikać sytuacji, w których jednoczesny dostęp do zasobów powoduje niespójności danych lub utratę komunikatów.
## Kompilacja oraz uruchomienie programu
- Kompilacja serwera g++ server.cpp -o server -pthread
- Kompilacja klienta: g++ client.cpp -o client -pthread
- Uruchomienie serwera: ./server
- Uruchomienie klientów (w osobnych terminalach): ./client
- Program testowany na systemie Ubuntu 24.04.1 LTS z użyciem powłoki Bash.
## Wątki
Wątek główny serwera:
- Akceptuje nowe połączenia od klientów (funkcja main()).
Wątki obsługi klientów:
- Każde połączenie klienta uruchamia nowy wątek wykonujący funkcję handle_client(). Wątki te:
- Odbierają wiadomości od przypisanego klienta,
- Rozsyłają komunikaty do pozostałych uczestników,
- Usuwają klienta z listy po rozłączeniu.
## Sekcje krytyczne
Dostęp do listy klientów:
- Chroniony przez mutex clients_mutex.
- Dodawanie nowego klienta przy akceptacji połączenia,
- Usuwanie klienta po rozłączeniu,
- Iteracja po liście klientów podczas rozsyłania wiadomości.
### Mechanizm synchronizacji:
- Dostęp do współdzielonej listy klientów jest chroniony przez mutex (clients_mutex), aby zapobiec jednoczesnej modyfikacji przez wiele wątków.
- Dodawanie i usuwanie klientów z listy oraz rozsyłanie wiadomości do wszystkich klientów odbywa się w sekcji krytycznej zabezpieczonej mutexem.
- Mutex blokowany jest tylko na czas krótkich operacji na liście, co minimalizuje ryzyko zakleszczeń i zapewnia spójność danych.
- Synchronizacja oparta jest na std::lock_guard, co gwarantuje automatyczne zwalnianie blokady po wyjściu z zakresu.
