#include <iostream>
#include <vector>
#include <thread>
#include <atomic>
#include <chrono>
#include <random>

using namespace std;

// SpinLock structure for thread synchronization
struct SpinLock {
    atomic<bool> locked;

    SpinLock() : locked(false) {}

    void lock() {
        while (true) {
            if (!locked.exchange(true)) {
                return; // Successfully locked
            }
            // Wait if someone else holds the lock
            this_thread::yield();
        }
    }

    void unlock() {
        locked.store(false);
    }
};

class DiningPhilosophers {
private:
    int num_philosophers; // Number of philosophers
    int max_iterations; //Number of maks iterations (dinings)
    vector<SpinLock> forks; // Fork (lock) vectors
    SpinLock console_lock;  // Use SpinLock for console synchronization
    mt19937 rng;  // Random number generator

    // Only one thread at a time can write to the console
    void log(int philosopher_id, const string& state) {
        console_lock.lock();
        cout << "Philosopher " << philosopher_id << " " << state << endl;
        console_lock.unlock();
    }

    // Generate random duration between 500-1500 milliseconds
    chrono::milliseconds random_duration() {
        static uniform_int_distribution<int> dist(500, 1500);
        return chrono::milliseconds(dist(rng));
    }

    void philosopher(int phil_no, int& iterations) {
        while (iterations < max_iterations) {
            // Thinking
            log(phil_no, "is thinking");
            this_thread::sleep_for(random_duration());

            log(phil_no, "is trying to eat");

            // Alternate fork pickup order to prevent deadlock
            if (phil_no % 2 == 0) {
                // Even philosophers pick up left fork first
                log(phil_no, "is trying to pick up left fork");
                forks[phil_no].lock();
                log(phil_no, "picked up left fork");
                
                log(phil_no, "is trying to pick up right fork");
                forks[(phil_no + 1) % num_philosophers].lock();
                log(phil_no, "picked up right fork");
            } else {
                // Odd philosophers pick up right fork first
                log(phil_no, "is trying to pick up right fork");
                forks[(phil_no + 1) % num_philosophers].lock();
                log(phil_no, "picked up right fork");
                
                log(phil_no, "is trying to pick up left fork");
                forks[phil_no].lock();
                log(phil_no, "picked up left fork");
            }

            // Eating
            log(phil_no, "is eating");
            this_thread::sleep_for(random_duration());

            // Putting down forks
            log(phil_no, "is putting down right fork");
            forks[(phil_no + 1) % num_philosophers].unlock();
            log(phil_no, "put down right fork");

            log(phil_no, "is putting down left fork");
            forks[phil_no].unlock();
            log(phil_no, "put down left fork");

            iterations++;
        }

        log(phil_no, "finished");
    }

public:
    DiningPhilosophers(int n, int iterations) : 
        num_philosophers(n), 
        max_iterations(iterations), 
        forks(n),
        rng(random_device{}()) // Seed with a random device
    {}

    void start() {
        vector<thread> philosophers;
        vector<int> iterations_count(num_philosophers, 0);

        // Create philosopher threads
        for (int i = 0; i < num_philosophers; ++i) {
            philosophers.emplace_back(&DiningPhilosophers::philosopher, this, i, ref(iterations_count[i]));
        }

        // Wait for threads to complete
        for (auto& p : philosophers) {
            p.join();
        }
    }
};

int main() {
    int num_philosophers;
    int max_iterations;

    cout << "Enter number of philosophers (minimum 2): ";
    cin >> num_philosophers;
    
    // Ensuring a minimum number of philosophers
    num_philosophers = max(2, num_philosophers);    

    cout << "Enter maximum number of iterations (minimum 1): ";
    cin >> max_iterations;
    
    // Ensuring a minimum number of iterations
    max_iterations = max(1, max_iterations);

    // Create and start dining philosophers simulation
    try {
        DiningPhilosophers dining(num_philosophers, max_iterations);
        dining.start();
    } catch (const exception& e) {
        cout << "An error occurred: " << e.what() << endl;
        return 1;
    }

    return 0;
}