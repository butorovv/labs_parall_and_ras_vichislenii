#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <random>
#include <atomic>
#include <mutex>

int balance_unsync = 0;

std::atomic<int> balance_atomic(0);
int balance_mutex = 0;
std::mutex mtx;

void client_transaction_unsync(int id) {

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(-100, 100);

    for (int i = 0; i < 1000; ++i) {
        int amount = dis(gen);
        balance_unsync += amount;
    }
}

void client_transaction_atomic(int id) {

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(-100, 100);

    for (int i = 0; i < 1000; ++i) {
        int amount = dis(gen);
        balance_atomic += amount;
    }
}

void client_transaction_mutex(int id) {

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(-100, 100);

    for (int i = 0; i < 1000; ++i) {
        int amount = dis(gen);

        std::lock_guard<std::mutex> lock(mtx);
        balance_mutex += amount;
    }
}

void run_test(int num_threads, const std::string& test_name, void (*client_transaction)(int)) {

    auto start = std::chrono::high_resolution_clock::now();
    std::vector<std::thread> threads;

    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back(client_transaction, i);
    }
    for (auto& t : threads) {
        t.join();
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;

    std::cout << "����: " << test_name << " (" << num_threads << " �������)\n";
    std::cout << "  ����� ����������: " << elapsed.count() << " ������\n";

    if (test_name == "��� �������������") {
        std::cout << "  �������� ������: " << balance_unsync << "\n";
    }
    else if (test_name == "� �������������� std::atomic") {
        std::cout << "  �������� ������: " << balance_atomic << "\n";
    }
    else if (test_name == "� �������������� std::mutex") {
        std::cout << "  �������� ������: " << balance_mutex << "\n";
    }

    std::cout << "\n";
}

int main() {

    std::vector<int> thread_counts = { 2, 4, 8 };
    for (int num_threads : thread_counts) {

        balance_unsync = 0;
        balance_atomic = 0;
        balance_mutex = 0;

        run_test(num_threads, "��� �������������", client_transaction_unsync);
        run_test(num_threads, "� �������������� std::atomic", client_transaction_atomic);
        run_test(num_threads, "� �������������� std::mutex", client_transaction_mutex);

    }

    return 0;
}